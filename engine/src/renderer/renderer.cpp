#include "terra/core/assert.h"
#include "terra/core/logger.h"
#include "terrapch.h"

#include "terra/resources/resource_manager.h"
#include "terra/renderer/renderer_command.h"
#include "terra/renderer/renderer.h"
#include "terra/renderer/material.h"
#include "terra/helpers/string.h"

namespace terra {

Renderer::Renderer(WebGPUContext& ctx)
    : m_context(ctx)
    , m_queue(*ctx.get_queue())
{
    m_scene_data = create_scope<SceneData>();
}

Renderer::~Renderer() {
}

void Renderer::init() {

    WGPUTextureDescriptor depth_texture_desc = WGPU_TEXTURE_DESCRIPTOR_INIT;
	depth_texture_desc.label = "Z Buffer"_wgpu;
	depth_texture_desc.usage = WGPUTextureUsage_RenderAttachment;
	depth_texture_desc.size = { 640, 480, 1 };
	depth_texture_desc.format = m_depth_texture_format;
	WGPUTexture depth_texture = wgpuDeviceCreateTexture(m_context.get_native_device(), &depth_texture_desc);
	// Create the view of the depth texture manipulated by the rasterizer
	m_depth_texture_view = wgpuTextureCreateView(depth_texture, nullptr);
	
	// We can now release the texture and only hold to the view
	wgpuTextureRelease(depth_texture);
}

u64 Renderer::create_pipeline(const PipelineSpecification& spec) {
    // You could hash the spec if you want deduplication (optional)
    u64 id = m_next_pipeline_id++;

    // Fill in internal fields like surface format and depth view
    PipelineSpecification internal_spec = spec;
    internal_spec.surface_format = m_context.get_preferred_format();
    internal_spec.depth_view = m_depth_texture_view;

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
    m_stats.reset();
    std::tie(m_surface_texture, m_target_texture_view) = m_context.get_next_surface_view();
}

void Renderer::begin_scene(const Camera& camera) {
    m_scene_data->camera = &camera;

    // clears to clearColor
    m_current_pass = RendererCommand::begin_render_pass(
        m_queue, 
        m_target_texture_view, 
        WGPULoadOp_Clear
    );
}

void Renderer::end_scene() {
    RendererCommand::end_render_pass(m_queue);
    m_current_pass = nullptr;
}

void Renderer::submit(const ref<Mesh>& mesh, const ref<MaterialInstance>& material_instance, const glm::mat4& transform) {
    if (!m_current_pass) return;

    // TODO: Set view projection matrix from m_scene_data->camera
    // This will require changes in material/shader system
    // For now, the user example doesn't use a camera matrix

    material_instance->get_pipeline()->bind(m_current_pass);

    const auto& vb = mesh->get_vertex_buffer();
    const auto& ib = mesh->get_index_buffer();

    wgpuRenderPassEncoderSetVertexBuffer(
        m_current_pass, 
        0, 
        vb.buffer, 
        0, 
        vb.size
    );

    wgpuRenderPassEncoderSetIndexBuffer(
        m_current_pass,
        ib.buffer,
        WGPUIndexFormat_Uint32,
        0,
        ib.size
    );

    material_instance->bind(m_current_pass);

    wgpuRenderPassEncoderDrawIndexed(
        m_current_pass, 
        mesh->get_index_count(), 
        1, 
        0, 
        0,
        0
    );

    m_stats.draw_calls++;
    m_stats.mesh_count++;
    m_stats.vertex_count += mesh->get_vertex_count();
    m_stats.index_count += mesh->get_index_count();
}


void Renderer::begin_ui_pass() {
    // load what we just drew, draw UI on top
    m_current_pass = RendererCommand::begin_render_pass(
        m_queue, 
        m_target_texture_view, 
        WGPULoadOp_Load
    );
}

void Renderer::end_ui_pass() {
    RendererCommand::end_render_pass(m_queue);
    m_current_pass = nullptr;
}

void Renderer::end_frame() {
    m_context.swap_buffers();
    m_queue.poll(false);
}


void Renderer::clear_color(f32 r, f32 g, f32 b, f32 a) {
    RendererCommand::set_clear_color(r, g, b, a);
}

RenderPass* Renderer::create_render_pass(const RenderPassDesc& desc) {
    m_render_passes.push_back(std::make_unique<RenderPass>(desc, m_queue));
    return m_render_passes.back().get();
}

}
