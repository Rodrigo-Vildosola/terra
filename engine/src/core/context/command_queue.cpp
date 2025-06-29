#include "terra/core/context/command_queue.h"
#include "terra/core/context/context_utils.h"
#include "terra/renderer/render_pass.h"
#include "terra/helpers/string.h"

namespace terra {

CommandQueue::CommandQueue(const CommandQueueProps& props) {}

CommandQueue::~CommandQueue() {

    if (m_queue) wgpuQueueRelease(m_queue);
}


void CommandQueue::init(const WGPUDevice device) {
    m_device = device;
    m_queue = wgpuDeviceGetQueue(device);

    auto on_queue_work_done = +[](
        WGPUQueueWorkDoneStatus status,
        void* /* pUserData1*/,
        void* /* pUserData2 */)
    {
		TR_CORE_INFO("Queued work finished with status: 0x{:X}", (u32) status);
	};

    WGPUQueueWorkDoneCallbackInfo callback_info = WGPU_QUEUE_WORK_DONE_CALLBACK_INFO_INIT;
    callback_info.nextInChain = nullptr;
    callback_info.mode = WGPUCallbackMode_AllowSpontaneous;
    callback_info.callback = on_queue_work_done;
    callback_info.userdata1 = nullptr;
    callback_info.userdata2 = nullptr;
	wgpuQueueOnSubmittedWorkDone(m_queue, callback_info);
}


void CommandQueue::begin_frame(std::string_view label) {
    TR_CORE_ASSERT(!m_frame_active, "Command encoder already active!");

    WGPUCommandEncoderDescriptor desc = WGPU_COMMAND_ENCODER_DESCRIPTOR_INIT;
    desc.label = to_wgpu_string_view(label);
    m_encoder = wgpuDeviceCreateCommandEncoder(m_device, &desc);

    m_frame_active = true;
}

// command_queue.cpp (inside CommandQueue)
WGPURenderPassEncoder CommandQueue::create_render_pass(const RenderPassDesc& desc) {
    TR_CORE_ASSERT(m_frame_active, "Cannot create render pass without an active encoder");

    // --- Convert color attachments ---
    std::vector<WGPURenderPassColorAttachment> color_attachments(desc.color_attachments.size());

    for (u64 i = 0; i < desc.color_attachments.size(); ++i) {
        const auto& src = desc.color_attachments[i];

        WGPURenderPassColorAttachment dst = WGPU_RENDER_PASS_COLOR_ATTACHMENT_INIT;
        dst.view = src.view;
        dst.loadOp = src.load_op;
        dst.storeOp = src.store_op;
        dst.clearValue = src.clear_color;

        color_attachments[i] = dst;
    }

    // --- Render pass descriptor ---
    WGPURenderPassDescriptor render_pass_desc = WGPU_RENDER_PASS_DESCRIPTOR_INIT;
    render_pass_desc.label = to_wgpu_string_view(desc.name);
    render_pass_desc.colorAttachmentCount = (u32) color_attachments.size();
    render_pass_desc.colorAttachments = color_attachments.data();

    // --- Optional depth-stencil attachment ---
    if (desc.depth_stencil_attachment.view) {
        WGPURenderPassDepthStencilAttachment depth_attachment = WGPU_RENDER_PASS_DEPTH_STENCIL_ATTACHMENT_INIT;

        const auto& src = desc.depth_stencil_attachment;
    
        depth_attachment.view = src.view;
        depth_attachment.depthLoadOp = src.load_op;
        depth_attachment.depthStoreOp = src.store_op;
        depth_attachment.depthClearValue = src.clear_depth;
        depth_attachment.depthReadOnly = src.read_only_depth;

        render_pass_desc.depthStencilAttachment = &depth_attachment;
    }

    // --- Begin render pass ---
    m_render_pass_encoder = wgpuCommandEncoderBeginRenderPass(m_encoder, &render_pass_desc);
    return m_render_pass_encoder;
}


void CommandQueue::end_render_pass() {
    if (m_render_pass_encoder) {
        wgpuRenderPassEncoderEnd(m_render_pass_encoder);
        wgpuRenderPassEncoderRelease(m_render_pass_encoder);
        m_render_pass_encoder = nullptr;
    }
}




void CommandQueue::end_frame() {
    TR_CORE_ASSERT(m_frame_active, "No active command encoder!");

    WGPUCommandBufferDescriptor desc = WGPU_COMMAND_BUFFER_DESCRIPTOR_INIT;
    desc.label = "Command Buffer"_wgpu;
    WGPUCommandBuffer cmd_buffer = wgpuCommandEncoderFinish(m_encoder, &desc);

    wgpuCommandEncoderRelease(m_encoder);
    m_encoder = nullptr;

    wgpuQueueSubmit(m_queue, 1, &cmd_buffer);
    wgpuCommandBufferRelease(cmd_buffer);

    m_frame_active = false;
}

void CommandQueue::submit_now() {
    if (m_frame_active) {
        end_frame();
    }
}


void CommandQueue::add_marker(std::string_view label) {
    TR_CORE_ASSERT(m_frame_active, "Can't add marker: no active encoder.");
    wgpuCommandEncoderInsertDebugMarker(m_encoder, to_wgpu_string_view(label));
}


void CommandQueue::poll([[maybe_unused]] bool yield_to_browser) {
    wgpu_poll_events(m_device, false);
}


scope<CommandQueue> CommandQueue::create(const CommandQueueProps& props) {
    return create_scope<CommandQueue>(props);
}


}

