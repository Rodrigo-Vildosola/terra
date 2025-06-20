#pragma once

#include "terra/core/context/command_queue.h"

namespace terra {

struct RendererCommand {
    // start a sub‐pass, with a custom loadOp
    //   - if loadOp==WGPULoadOp_Clear, will clear to s_clear_color
    //   - if loadOp==WGPULoadOp_Load, will preserve existing pixels
    static WGPURenderPassEncoder begin_render_pass(CommandQueue& q, WGPUTextureView targetView, WGPULoadOp load_op);
    static void set_clear_color(f32 r, f32 g, f32 b, f32 a);
    static void end_render_pass(CommandQueue& q);

private:
    // store clear color between begin/end
    static WGPUColor s_clear_color;
};

} // namespace terra
