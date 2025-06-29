#include "terra/renderer/renderer_command.h"
#include "terra/renderer/render_pass.h"

namespace terra {

WGPURenderPassEncoder RendererCommand::begin_render_pass(CommandQueue& q, const RenderPassDesc& desc) {
    q.begin_frame(desc.name);
    q.add_marker("Begin Render Pass");
    return q.create_render_pass(desc);
}


void RendererCommand::end_render_pass(CommandQueue& q) {
    q.end_render_pass();
    q.add_marker("End Render Pass");
    q.end_frame();
}

} // namespace terra
