#include "terra/core/assert.h"
#include "terra/core/logger.h"
#include "terrapch.h"

#include "terra/resources/resource_manager.h"
#include "terra/renderer/renderer_command.h"
#include "terra/renderer/renderer.h"
#include "terra/renderer/material.h"

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
    // Nothing to do here for now
}

void Renderer::begin_frame() {
    std::tie(m_surface_texture, m_target_view) = m_context.get_next_surface_view();
}

void Renderer::begin_scene(const Camera& camera) {
    m_scene_data->camera = &camera;

    // clears to clearColor
    m_current_pass = RendererCommand::begin_render_pass(
        m_queue, 
        m_target_view, 
        WGPULoadOp_Clear
    );
}

void Renderer::end_scene() {
    RendererCommand::end_render_pass(m_queue);
    m_current_pass = nullptr;
}

void Renderer::submit(const ref<Mesh>& mesh, const ref<MaterialInstance>& material_instance, const glm::mat4& transform)
{
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
}


void Renderer::begin_ui_pass() {
    // load what we just drew, draw UI on top
    m_current_pass = RendererCommand::begin_render_pass(
        m_queue, 
        m_target_view, 
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

}
