#pragma once

#include "terra/core/context/command_queue.h"

namespace terra {

struct RendererCommand {
    // start a sub‐pass, with a custom loadOp
    //   - if loadOp==WGPULoadOp_Clear, will clear to s_clearColor
    //   - if loadOp==WGPULoadOp_Load, will preserve existing pixels
    static WGPURenderPassEncoder begin_render_pass(CommandQueue& q, WGPUTextureView targetView, WGPULoadOp load_op);
    static void set_clear_color(float r, float g, float b, float a);
    static void end_render_pass(CommandQueue& q);

private:
    // store clear color between begin/end
    static WGPUColor s_clearColor;
};

} // namespace terra
