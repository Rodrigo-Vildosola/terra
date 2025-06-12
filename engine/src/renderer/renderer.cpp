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
    // compile pipelines, create bind-groups, etc.
}

void Renderer::begin_frame() {
    auto [surface_texture, target_view] = m_context.get_next_surface_view();
    if (!target_view) return;

    RendererCommand::begin_render_pass(m_queue, target_view);
}

void Renderer::clear_color(float r, float g, float b, float a) {
    RendererCommand::set_clear_color(r,g,b,a);
}

void Renderer::end_frame() {
    RendererCommand::end_render_pass(m_queue);
    m_context.swap_buffers();
    m_queue.poll();
}

}
