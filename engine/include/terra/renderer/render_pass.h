#pragma once


#include "terrapch.h"

namespace terra {

class CommandQueue;

struct RenderPassAttachment {
    WGPUTextureView view = nullptr;

    WGPULoadOp load_op = WGPULoadOp_Clear;
    WGPUStoreOp store_op = WGPUStoreOp_Store;

    WGPUColor clear_color = {0.0f, 0.0f, 0.0f, 1.0f}; // Used for color
    float clear_depth = 1.0f;                        // Used for depth
    uint32_t clear_stencil = 0;                      // Optional

    bool read_only_depth = false;                    // For depth-only passes
};

struct RenderPassDesc {
    std::string_view name = "UnnamedPass";

    std::vector<RenderPassAttachment> color_attachments;
    RenderPassAttachment depth_stencil_attachment;
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
