#include "terra/core/base.h"
#include "terrapch.h"
#include "terra/renderer/renderer_command.h"
#include "terra/renderer/renderer.h"

namespace terra {

Renderer::Renderer(WebGPUContext& ctx)
    : m_context(ctx)
    , m_queue(*ctx.get_queue())
{}

Renderer::~Renderer() {}


// Allocate static context
void Renderer::init() {

    PipelineSpecification spec;
    spec.shader_path = "assets/shaders/triangle.wgsl";
    spec.surface_format = m_context.get_preferred_format();
    spec.vertex_entry = "vs_main";
    spec.fragment_entry = "fs_main";
    spec.uniform_buffer_size = 0;

    m_pipeline = create_scope<Pipeline>(m_context, spec);
    // compile pipelines, create bind-groups, etc.
}

WGPURenderPassEncoder Renderer::begin_frame() {
    auto [surface_texture, target_view] = m_context.get_next_surface_view();
    if (!target_view) return nullptr;

    return RendererCommand::begin_render_pass(m_queue, target_view);
}

WGPURenderPassEncoder Renderer::get_render_pass_encoder() {
    return m_queue.get_render_pass_encoder();
}

void Renderer::clear_color(float r, float g, float b, float a) {
    RendererCommand::set_clear_color(r,g,b,a);
}

void Renderer::end_frame() {
    RendererCommand::end_render_pass(m_queue);
    m_context.swap_buffers();
    m_queue.poll();
}

void Renderer::draw_triangle() {
    WGPURenderPassEncoder render_pass = get_render_pass_encoder();
    if (!render_pass) return;

    m_pipeline->bind(render_pass); // this does setPipeline()

    // draw 3 vertices as a triangle
    wgpuRenderPassEncoderDraw(render_pass, 3, 1, 0, 0);
}


}
