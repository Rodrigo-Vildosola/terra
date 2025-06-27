#pragma once

#include <vector>
#include <string>
#include <webgpu/webgpu.h>
#include "terra/core/context/command_queue.h"

namespace terra {

struct RenderPassAttachment {
    WGPUTextureView view = nullptr;
    WGPULoadOp load_op = WGPULoadOp_Clear;
    WGPUStoreOp store_op = WGPUStoreOp_Store;
    WGPUColor clear_color = {0, 0, 0, 1};
};

struct RenderPassDesc {
    std::vector<RenderPassAttachment> color_attachments;
    // TODO: Add depth, resolve, etc.
    std::string name;
};

class RenderPass {
public:
    RenderPass(const RenderPassDesc& desc, CommandQueue& queue);
    ~RenderPass();

    void begin();
    void end();

    WGPURenderPassEncoder get_encoder() const { return m_encoder; }

    void add_dependency(RenderPass* dependency);
    const std::vector<RenderPass*>& get_dependencies() const { return m_dependencies; }
    const RenderPassDesc& get_desc() const { return m_desc; }

private:
    RenderPassDesc m_desc;
    CommandQueue& m_queue;
    WGPURenderPassEncoder m_encoder = nullptr;
    std::vector<RenderPass*> m_dependencies;
};

} // namespace terra 
