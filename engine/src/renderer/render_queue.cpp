#include "terra/renderer/render_queue.h"
#include "terra/helpers/string.h"

namespace terra {

RenderQueue::RenderQueue(const RenderQueueProps& props) {}

RenderQueue::~RenderQueue() {

    if (m_command_queue) {
        wgpuQueueRelease(m_command_queue);
    }
}


void RenderQueue::init(const WGPUDevice device) {

    m_command_queue = wgpuDeviceGetQueue(device);

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
	wgpuQueueOnSubmittedWorkDone(m_command_queue, callback_info);

    WGPUCommandEncoderDescriptor encoder_desc = {};
	encoder_desc.nextInChain = nullptr;
	encoder_desc.label = "My command encoder"_wgpu;
	m_encoder = wgpuDeviceCreateCommandEncoder(device, &encoder_desc);

    wgpuCommandEncoderInsertDebugMarker(m_encoder, "Do one thing"_wgpu);
    wgpuCommandEncoderInsertDebugMarker(m_encoder, "Do another thing"_wgpu);

    WGPUCommandBufferDescriptor cmd_buffer_desc = {};
    cmd_buffer_desc.nextInChain = nullptr;
    cmd_buffer_desc.label = "Command Buffer"_wgpu;
    WGPUCommandBuffer command = wgpuCommandEncoderFinish(m_encoder, &cmd_buffer_desc);
	wgpuCommandEncoderRelease(m_encoder); // release encoder after it's finished

    TR_CORE_INFO("Submitting command...");
    wgpuQueueSubmit(m_command_queue, 1, &command);
	wgpuCommandBufferRelease(command);
	for (int i = 0 ; i < 5 ; ++i) {
		TR_CORE_INFO("Tick/Poll device...");
#if defined(WEBGPU_BACKEND_DAWN)
		wgpuDeviceTick(device);
#elif defined(WEBGPU_BACKEND_WGPU)
		wgpuDevicePoll(device, false, nullptr);
#elif defined(WEBGPU_BACKEND_EMSCRIPTEN)
		emscripten_sleep(100);
#endif
	}
}

scope<RenderQueue> RenderQueue::create(const RenderQueueProps& props) {
    return create_scope<RenderQueue>(props);
}


}

