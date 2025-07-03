#include "terra/core/assert.h"
#include "terra/core/base.h"
#include "terra/core/logger.h"
#include "terra/debug/profiler.h"
#include "terrapch.h"

#include "terra/resources/resource_manager.h"
#include "terra/renderer/renderer_command.h"
#include "terra/renderer/renderer.h"
#include "terra/renderer/material.h"
#include "terra/helpers/string.h"

namespace terra {

Renderer::Renderer(WebGPUContext& ctx) : m_context(ctx), m_queue(*ctx.get_queue()) {
    m_scene_data = create_scope<SceneData>();
}

Renderer::~Renderer() {
}

void Renderer::init() {
    m_depth_texture_format = wgpu::TextureFormat::Depth24Plus;

    auto [fb_width, fb_height] = m_context.get_framebuffer_size();

    on_resize(fb_width, fb_height);
}

u64 Renderer::create_pipeline(const PipelineSpecification& spec) {
    PROFILE_FUNCTION();

    // You could hash the spec if you want deduplication (optional)
    u64 id = m_next_pipeline_id++;

    // Fill in internal fields like surface format and depth view
    PipelineSpecification internal_spec = spec;
    internal_spec.surface_format = m_context.get_preferred_format();
    internal_spec.depth_view = m_depth_texture_view;
    internal_spec.depth_format = m_depth_texture_format;

    ref<Pipeline> pipeline = create_ref<Pipeline>(m_context, internal_spec);
    m_pipeline_cache[id] = pipeline;

    return id;
}


ref<Pipeline> Renderer::get_pipeline(u64 id) const {
    auto it = m_pipeline_cache.find(id);
    if (it != m_pipeline_cache.end())
        return it->second;
    TR_CORE_ASSERT(false, "Pipeline ID not found!");
    return nullptr;
}

void Renderer::begin_frame() {
    PROFILE_FUNCTION();

    m_stats.reset();
    m_target_texture_view = m_context.get_next_surface_view();
}

void Renderer::begin_scene(const Camera& camera) {
    PROFILE_FUNCTION();

    m_scene_data->camera = &camera;

    RenderPassDesc scene_pass;
    scene_pass.name = "MainScene";

    // Color attachment
    RenderPassAttachment color_attachment;
    color_attachment.view = m_target_texture_view;
    color_attachment.load_op = wgpu::LoadOp::Clear;
    color_attachment.store_op = wgpu::StoreOp::Store;
    color_attachment.clear_color = m_clear_color;
    scene_pass.color_attachments.push_back(color_attachment);

    // Depth attachment
    RenderPassAttachment depth_attachment;
    depth_attachment.view = m_depth_texture_view;
    depth_attachment.load_op = wgpu::LoadOp::Clear;
    depth_attachment.store_op = wgpu::StoreOp::Store;
    depth_attachment.clear_depth = 1.0f;
    depth_attachment.read_only_depth = false;
    scene_pass.depth_stencil_attachment = depth_attachment;

    // clears to clearColor
    m_current_pass = RendererCommand::begin_render_pass(m_queue, scene_pass);

}

void Renderer::end_scene() {
    PROFILE_FUNCTION();

    // 1) For each batch, update (or allocate) its GPU buffer & draw
    for (auto& b : m_draw_batches) {
        if (b.instance_count == 0) continue;

        u64 needed = b.instance_data.size();
        // allocate or resize
        if (!b.instance_buffer || needed > b.buffer_capacity) {
            // if (b.instance_buffer) wgpuBufferRelease(b.instance_buffer);

            b.instance_buffer = Buffer::create_storage_buffer(
                m_context,
                b.instance_data.data(),
                needed,
                b.binding,
                "Instance Storage Buffer"
            ).buffer;

            b.buffer_capacity = needed;

        } else {
            m_queue.get_native_queue().WriteBuffer(b.instance_buffer, 0, b.instance_data.data(), needed);
            // just update contents
        }

        // bind storage buffer
        b.material->bind_storage_buffer(b.group, b.binding, b.instance_buffer);

        b.material->bind(m_current_pass);

        auto const& vb = b.mesh->get_vertex_buffer();
        auto const& ib = b.mesh->get_index_buffer();

        m_current_pass.SetVertexBuffer(0, vb.buffer, 0, vb.size);

        m_current_pass.SetIndexBuffer(ib.buffer, ib.format, 0, ib.size);

        m_current_pass.DrawIndexed(b.mesh->get_index_count(), b.instance_count, 0, 0, 0);

        m_stats.draw_calls++;
        m_stats.mesh_count  += 1;
        m_stats.vertex_count += b.mesh->get_vertex_count() * b.instance_count;
        m_stats.index_count  += b.mesh->get_index_count()  * b.instance_count;
    }

    // 2) clear batches
    m_draw_batches.clear();

    // 3) end the pass
    RendererCommand::end_render_pass(m_queue);
    m_current_pass = nullptr;
}

void Renderer::submit(const ref<Mesh>& mesh, const ref<MaterialInstance>& material, const void* instance, u32 i_size, u32 binding, u32 group) {
    PROFILE_FUNCTION();

    if (!m_current_pass) return;

    for (auto& b : m_draw_batches) {
        if (b.mesh == mesh
         && b.material == material
         && b.binding == binding
         && b.group == group
         && b.instance_stride == i_size)
        {
            // append raw bytes
            const u8* src = (const u8*) instance;
            b.instance_data.insert(
                b.instance_data.end(),
                src, src + i_size
            );
            b.instance_count++;
            return;
        }
    }

    DrawBatch nb;
    nb.mesh            = mesh;
    nb.material        = material;
    nb.binding         = binding;
    nb.group           = group;
    nb.instance_stride = i_size;
    nb.instance_count  = 1;
    nb.instance_data.resize(i_size);
    memcpy(nb.instance_data.data(), instance, i_size);
    nb.instance_buffer     = nullptr;
    nb.buffer_capacity     = 0;
    m_draw_batches.push_back(nb);
}


void Renderer::begin_ui_pass() {
    PROFILE_FUNCTION();

    // load what we just drew, draw UI on top
    RenderPassDesc ui_pass;
    ui_pass.name = "UIPass";

    // Only color, no depth
    RenderPassAttachment color_attachment;
    color_attachment.view = m_target_texture_view;
    color_attachment.load_op = wgpu::LoadOp::Load;
    color_attachment.store_op = wgpu::StoreOp::Store;
    ui_pass.color_attachments.push_back(color_attachment);

    m_current_pass = RendererCommand::begin_render_pass(m_queue, ui_pass);
}

void Renderer::end_ui_pass() {
    PROFILE_FUNCTION();

    RendererCommand::end_render_pass(m_queue);
    m_current_pass = nullptr;
}

void Renderer::end_frame() {
    PROFILE_FUNCTION();

    m_context.swap_buffers();
    m_queue.poll(false);
}


void Renderer::clear_color(f32 r, f32 g, f32 b, f32 a) {
    PROFILE_FUNCTION();

    m_clear_color = {
        .r = r,
        .g = g,
        .b = b,
        .a = a
    };
}

// RenderPass* Renderer::create_render_pass(const RenderPassDesc& desc) {
//     m_render_passes.push_back(create_scope<RenderPass>(desc, m_queue));
//     return m_render_passes.back().get();
// }

void Renderer::on_resize(u32 width, u32 height) {
    TR_CORE_INFO("Resizing renderer to {}x{}", width, height);

    const auto& device = m_context.get_native_device();

    wgpu::TextureDescriptor depth_texture_desc = {};
    depth_texture_desc.label = "Z Buffer";
    depth_texture_desc.usage = wgpu::TextureUsage::RenderAttachment;
    depth_texture_desc.size = { width, height, 1 };
    depth_texture_desc.format = m_depth_texture_format;
    
    wgpu::Texture depth_texture = device.CreateTexture(&depth_texture_desc);

    m_depth_texture_view = depth_texture.CreateView();
}

void Renderer::invalidate_surface_view() {
    m_target_texture_view = nullptr;
}

}
