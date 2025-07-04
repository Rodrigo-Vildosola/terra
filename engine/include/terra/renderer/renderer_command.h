#pragma once

#include "terra/core/context/command_queue.h"

namespace terra {

struct RendererCommand {
    // start a sub‐pass, with a custom loadOp
    //   - if loadOp==wgpu::LoadOp::Clear, will clear to s_clear_color
    //   - if loadOp==wgpu::LoadOp::Load, will preserve existing pixels
    static wgpu::RenderPassEncoder begin_render_pass(CommandQueue& q, const RenderPassDesc& desc);
    static void set_clear_color(f32 r, f32 g, f32 b, f32 a);
    static void end_render_pass(CommandQueue& q);

};

} // namespace terra
