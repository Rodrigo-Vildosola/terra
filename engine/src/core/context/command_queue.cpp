#include "terra/core/context/command_queue.h"
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

    WGPUQueueWorkDoneCallbackInfo callback_info = {};
    callback_info.nextInChain = nullptr;
    callback_info.mode = WGPUCallbackMode_AllowSpontaneous;
    callback_info.callback = on_queue_work_done;
    callback_info.userdata1 = nullptr;
    callback_info.userdata2 = nullptr;
	wgpuQueueOnSubmittedWorkDone(m_queue, callback_info);
}


void CommandQueue::begin_frame(std::string_view label) {
    TR_CORE_ASSERT(!m_frame_active, "Command encoder already active!");

    WGPUCommandEncoderDescriptor desc = {};
    desc.label = to_wgpu_string_view(label); // use your `_wgpu` literal or helper
    m_encoder = wgpuDeviceCreateCommandEncoder(m_device, &desc);

    m_frame_active = true;
}

// command_queue.cpp (inside CommandQueue)
void CommandQueue::create_render_pass(WGPUTextureView target, WGPUColor clear_color) {
    TR_CORE_ASSERT(m_frame_active, "Cannot create render pass without an active encoder");

    // Color attachment setup
    WGPURenderPassColorAttachment color_attachment = {};
    color_attachment.view = target;
    color_attachment.resolveTarget = nullptr;
    color_attachment.loadOp = WGPULoadOp_Clear;
    color_attachment.storeOp = WGPUStoreOp_Store;
    color_attachment.clearValue = clear_color;

#ifndef WEBGPU_BACKEND_WGPU
    // Optional: Needed by Dawn backend, not supported on wgpu-native yet
    color_attachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
#endif

    // Render pass descriptor
    WGPURenderPassDescriptor render_pass_desc = {};
    render_pass_desc.nextInChain = nullptr;
    render_pass_desc.colorAttachmentCount = 1;
    render_pass_desc.colorAttachments = &color_attachment;
    render_pass_desc.depthStencilAttachment = nullptr;
    render_pass_desc.timestampWrites = nullptr;

    // Begin → end immediately (just clears screen for now)
    WGPURenderPassEncoder pass = wgpuCommandEncoderBeginRenderPass(m_encoder, &render_pass_desc);
    wgpuRenderPassEncoderEnd(pass);
    wgpuRenderPassEncoderRelease(pass);
}



void CommandQueue::end_frame() {
    TR_CORE_ASSERT(m_frame_active, "No active command encoder!");

    WGPUCommandBufferDescriptor desc = {};
    desc.label = "Command Buffer"_wgpu;
    WGPUCommandBuffer cmdBuffer = wgpuCommandEncoderFinish(m_encoder, &desc);

    wgpuCommandEncoderRelease(m_encoder);
    m_encoder = nullptr;

    wgpuQueueSubmit(m_queue, 1, &cmdBuffer);
    wgpuCommandBufferRelease(cmdBuffer);

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


void CommandQueue::poll() {
    #if defined(WEBGPU_BACKEND_DAWN)
        wgpuDeviceTick(m_device);
    #elif defined(WEBGPU_BACKEND_WGPU)
        wgpuDevicePoll(m_device, false, nullptr);
    #endif
}


scope<CommandQueue> CommandQueue::create(const CommandQueueProps& props) {
    return create_scope<CommandQueue>(props);
}


}

