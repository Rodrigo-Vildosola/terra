#pragma once

#include "terra/core/context/command_queue.h"

namespace terra {

struct RendererCommand {
    // these wrap CommandQueue calls and low-level WebGPU API
    static void begin_render_pass(CommandQueue& q, WGPUTextureView targetView);
    static void set_clear_color(float r, float g, float b, float a);
    static void end_render_pass(CommandQueue& q);

private:
    // store clear color between begin/end
    static WGPUColor s_clearColor;
};

} // namespace terra
