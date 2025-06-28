#include "terra/renderer/render_pass.h"
#include "terra/core/assert.h"

namespace terra {

RenderPass::RenderPass(const RenderPassDesc& desc, CommandQueue& queue)
    : m_desc(desc), m_queue(queue) {}

RenderPass::~RenderPass() {
    // No explicit cleanup needed for encoder; handled by end()
}

void RenderPass::begin() {
    TR_CORE_ASSERT(m_encoder == nullptr, "RenderPass already begun!");
    // For now, only support one color attachment
    TR_CORE_ASSERT(!m_desc.color_attachments.empty());
    const auto& att = m_desc.color_attachments[0];
    // m_queue.begin_frame(m_desc.name.c_str());
    // m_encoder = m_queue.create_render_pass(att.view, att.clear_color, att.load_op);
}

void RenderPass::end() {
    TR_CORE_ASSERT(m_encoder != nullptr, "RenderPass not begun!");
    // m_queue.end_render_pass();
    // m_queue.end_frame();
    m_encoder = nullptr;
}

void RenderPass::add_dependency(RenderPass* dependency) {
    m_dependencies.push_back(dependency);
}

} // namespace terra 
