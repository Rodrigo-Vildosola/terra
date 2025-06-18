#include "terra/core/context/command_queue.h"
#include "terra/core/context/context_utils.h"
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
		TR_CORE_INFO("Queued work finished with status: 0x{:X}", (u32)status);
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
    desc.label = to_wgpu_string_view(label); // use your `_wgpu` literal or helper
    m_encoder = wgpuDeviceCreateCommandEncoder(m_device, &desc);

    m_frame_active = true;
}

// command_queue.cpp (inside CommandQueue)
WGPURenderPassEncoder CommandQueue::create_render_pass(WGPUTextureView target, WGPUColor clear_color, WGPULoadOp load_op) {
    TR_CORE_ASSERT(m_frame_active, "Cannot create render pass without an active encoder");

    // Color attachment setup
    WGPURenderPassColorAttachment color_attachment = WGPU_RENDER_PASS_COLOR_ATTACHMENT_INIT;
    color_attachment.view = target;
    color_attachment.loadOp = load_op;
    color_attachment.storeOp = WGPUStoreOp_Store;
    color_attachment.clearValue = clear_color;

    // Render pass descriptor
    WGPURenderPassDescriptor render_pass_desc = WGPU_RENDER_PASS_DESCRIPTOR_INIT;
    render_pass_desc.colorAttachmentCount = 1;
    render_pass_desc.colorAttachments = &color_attachment;

    // Begin → end immediately (just clears screen for now)
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

