#include "terra/renderer/renderer_command.h"
#include <webgpu/webgpu.hpp>

namespace terra {

WGPUColor RendererCommand::s_clear_color = {0, 0, 0, 1};

WGPURenderPassEncoder RendererCommand::begin_render_pass(CommandQueue& q, WGPUTextureView view, WGPULoadOp load_op) {
    q.begin_frame("Main Frame");
    q.add_marker("Begin Render Pass");
    return q.create_render_pass(view, s_clear_color, load_op);
}


void RendererCommand::set_clear_color(float r, float g, float b, float a) {
    s_clear_color = {
        .r = r,
        .g = g,
        .b = b,
        .a = a
    };
}

void RendererCommand::end_render_pass(CommandQueue& q) {
    q.end_render_pass();
    q.add_marker("End Render Pass");
    q.end_frame();
}

} // namespace terra



// 1. total enviados (por whatsapp)
// 2. total recibidos (resto errores)
// 3. total acusaron recibo (es decir conversaron por whatsapp, habla de la gestión de la ejecutiva y proactividad del paciente)
// 4. Total agendados
