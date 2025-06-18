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
    // compile pipelines, create bind-groups, etc.
}

void Renderer::begin_frame() {
    auto [surface_texture, target_view] = m_context.get_next_surface_view();
    if (!target_view) return;

    m_render_pass = RendererCommand::begin_render_pass(m_queue, target_view);
}

WGPURenderPassEncoder Renderer::get_render_pass_encoder() {
    return m_queue.get_render_pass_encoder();
}

void Renderer::clear_color(float r, float g, float b, float a) {
    RendererCommand::set_clear_color(r, g, b, a);
}

void Renderer::end_frame() {
    if (m_render_pass)
        RendererCommand::end_render_pass(m_queue);
    m_context.swap_buffers();
    m_queue.poll(false);
}

void Renderer::draw() {
    if (!m_render_pass) return;
    WGPURenderPassEncoder render_pass = get_render_pass_encoder();
    if (!render_pass) return;

    m_pipeline->bind(render_pass); // this does setPipeline()

    // draw 3 vertices as a triangle
    wgpuRenderPassEncoderSetVertexBuffer(render_pass, 0, m_vertex_buffer, 0, WGPU_WHOLE_SIZE);

    wgpuRenderPassEncoderDraw(render_pass, m_vertex_count, 1, 0, 0);
}


}
