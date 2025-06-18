#include "terra/core/base.h"
#include "terrapch.h"
#include "terra/renderer/renderer_command.h"
#include "terra/renderer/renderer.h"

namespace terra {

Renderer::Renderer(WebGPUContext& ctx)
    : m_context(ctx)
    , m_queue(*ctx.get_queue())
{}

Renderer::~Renderer() {
    if (m_vertex_buffer) wgpuBufferRelease(m_vertex_buffer);

}


// Allocate static context
void Renderer::init() {

    std::vector<f32> vertex_data = {
        // Triangle 1
        -0.45f, 0.5f,
        0.45f, 0.5f,
        0.0f, -0.5f,

        // Triangle 2
        0.47f, 0.47f,
        0.25f, 0.0f,
        0.69f, 0.0f
    };

    m_vertex_count = (u32)(vertex_data.size() / 2);
    
    m_vertex_buffer = Buffer::create(
        m_context.get_native_device(),
        m_queue.get_native_queue(),
        vertex_data.data(),
        vertex_data.size() * sizeof(f32),
        WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst,
        "Vertex Buffer"
    );

    PipelineSpecification spec;
    spec.shader_path = "assets/shaders/triangle.wgsl";
    spec.surface_format = m_context.get_preferred_format();
    spec.vertex_entry = "vs_main";
    spec.fragment_entry = "fs_main";
    spec.uniform_buffer_size = 0;

    m_pipeline = create_scope<Pipeline>(m_context, spec);
}

void Renderer::begin_frame() {
    std::tie(m_surface_texture, m_target_view) = m_context.get_next_surface_view();
}

void Renderer::begin_scene_pass() {
    // clears to clearColor
    m_current_pass =
      RendererCommand::begin_render_pass(m_queue, m_target_view, WGPULoadOp_Clear);
}

void Renderer::end_scene_pass() {
    RendererCommand::end_render_pass(m_queue);
    m_current_pass = nullptr;
}

void Renderer::begin_ui_pass() {
    // load what we just drew, draw UI on top
    m_current_pass =
      RendererCommand::begin_render_pass(m_queue, m_target_view, WGPULoadOp_Load);
}

void Renderer::end_ui_pass() {
    RendererCommand::end_render_pass(m_queue);
    m_current_pass = nullptr;
}

void Renderer::end_frame() {
    // present once
    m_context.swap_buffers();
    m_queue.poll(false);
}


void Renderer::clear_color(float r, float g, float b, float a) {
    RendererCommand::set_clear_color(r, g, b, a);
}

void Renderer::draw() {
    if (!m_current_pass) return;

    m_pipeline->bind(m_current_pass); // this does setPipeline()

    // draw 3 vertices as a triangle
    wgpuRenderPassEncoderSetVertexBuffer(m_current_pass, 0, m_vertex_buffer, 0, WGPU_WHOLE_SIZE);

    wgpuRenderPassEncoderDraw(m_current_pass, m_vertex_count, 1, 0, 0);
}


}
