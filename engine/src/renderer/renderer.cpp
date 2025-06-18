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
        // x0,  y0,  r0,  g0,  b0
        -0.45f, 0.5f, 1.0f, 1.0f, 0.0f, // (yellow)

        // x1,  y1,  r1,  g1,  b1
        0.45f, 0.5f, 1.0f, 0.0f, 1.0f, // (magenta)

        // ...
        0.0f,  -0.5f, 0.0f, 1.0f, 1.0f, // (cyan)
        0.47f, 0.47f, 1.0f, 0.0f, 0.0f, // (red)
        0.25f,  0.0f, 0.0f, 1.0f, 0.0f, // (green)
        0.69f,  0.0f, 0.0f, 0.0f, 1.0f  // (blue)
    };
    
    m_vertex_buffer = Buffer::create(
        m_context.get_native_device(),
        m_queue.get_native_queue(),
        vertex_data.data(),
        vertex_data.size() * sizeof(f32),
        WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst,
        "Vertex Buffer"
    );

    PipelineSpecification spec;
    spec.surface_format = m_context.get_preferred_format();
    spec.vertex_entry = "vs_main";
    spec.fragment_entry = "fs_main";
    spec.uniform_buffer_size = 0;

    VertexBufferLayoutSpec vb;
    vb.stride       = (2 + 3) * sizeof(f32);
    vb.step_mode    = WGPUVertexStepMode_Vertex;
    vb.attributes = {
        { 0, WGPUVertexFormat_Float32x2, 0 },
        { 1, WGPUVertexFormat_Float32x3, 2 * sizeof(f32) },
    };

    spec.vertex_buffers.push_back(vb);

    u32 nums_per_vertex = (u32) (spec.vertex_buffers[0].stride / sizeof(f32));
    m_vertex_count = (u32) vertex_data.size() / nums_per_vertex;

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
