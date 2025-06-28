#include "terra/renderer/renderer_command.h"
#include <webgpu/webgpu.hpp>

namespace terra {

WGPUColor RendererCommand::s_clear_color = {0, 0, 0, 1};

WGPURenderPassEncoder RendererCommand::begin_render_pass(CommandQueue& q, WGPUTextureView view, WGPULoadOp load_op) {
    q.begin_frame("Main Frame");
    q.add_marker("Begin Render Pass");
    return q.create_render_pass(view, s_clear_color, load_op);
}


void RendererCommand::set_clear_color(f32 r, f32 g, f32 b, f32 a) {
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
