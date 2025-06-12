#include "terra/renderer/renderer_command.h"
#include <webgpu/webgpu.hpp>

namespace terra {

WGPUColor RendererCommand::s_clearColor = {0, 0, 0, 1};

void RendererCommand::begin_render_pass(CommandQueue& q, WGPUTextureView view) {
    q.begin_frame("Main Frame");

    s_clearColor = {0.1f,0.2f,0.3f,1.0f}; // default or set via clear_color()
    q.add_marker("Begin Render Pass");
    q.create_render_pass(view, s_clearColor);
}

void RendererCommand::set_clear_color(float r, float g, float b, float a) {
    s_clearColor = {r, g, b, a};
}

void RendererCommand::end_render_pass(CommandQueue& q) {
    q.add_marker("End Render Pass");
    q.end_frame();
}

} // namespace terra
// 1. total enviados (por whatsapp)
// 2. total recibidos (resto errores)
// 3. total acusaron recibo (es decir conversaron por whatsapp, habla de la gestión de la ejecutiva y proactividad del paciente)
// 4. Total agendados
