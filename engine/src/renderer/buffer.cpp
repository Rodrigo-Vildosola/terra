
#include "terrapch.h"

#include "terra/core/context/context_utils.h"
#include "terra/core/context/context.h"
#include "terra/core/context/command_queue.h"
#include "terra/renderer/buffer.h"
#include "terra/helpers/string.h"
#include "terra/helpers/user_data.h"


namespace terra {

void fetch_buffer_data_sync(WGPUInstance instance, WGPUBuffer bufferB, std::function<void (const void *)> process_buffer_data) {
	// Context passed to `onBufferBMapped` through theuserdata pointer:

    request_userdata<bool> on_buffer_mapped_context;

    auto on_buffer_mapped = [](
		WGPUMapAsyncStatus status,
		WGPUStringView message,
		void* userdata1,
		void* /* userdata2 */
	) {
		request_userdata<bool>& context = *reinterpret_cast<request_userdata<bool>*>(userdata1);
		context.request_ended = true;
		if (status == WGPUMapAsyncStatus_Success) {
			context.result = true;
		} else {
			std::cout << "Could not map buffer B! Status: " << status << ", message: " << message.data << std::endl;
		}
	};

    WGPUBufferMapCallbackInfo callback_info = WGPU_BUFFER_MAP_CALLBACK_INFO_INIT;
    callback_info.mode = WGPUCallbackMode_AllowProcessEvents;
	callback_info.callback = on_buffer_mapped;
	callback_info.userdata1 = &on_buffer_mapped_context;

    wgpuBufferMapAsync(
		bufferB,
		WGPUMapMode_Read,
		0, // offset
		WGPU_WHOLE_MAP_SIZE,
		callback_info
	);

    wgpuInstanceProcessEvents(instance);
	while (!on_buffer_mapped_context.request_ended) {
		sleep_for_ms(200);
		wgpuInstanceProcessEvents(instance);
	}
	
	if (on_buffer_mapped_context.result) {
		const void* buffer_data = wgpuBufferGetConstMappedRange(bufferB, 0, WGPU_WHOLE_MAP_SIZE);
		process_buffer_data(buffer_data);
	}

}

WGPUBuffer Buffer::create(WGPUDevice device, WGPUQueue queue, const void *data, size_t size, WGPUBufferUsage usage, const char* label) {
    WGPUBufferDescriptor desc = WGPU_BUFFER_DESCRIPTOR_INIT;
    desc.size = size;
    desc.usage = usage;
    desc.mappedAtCreation = false;
    desc.label = label ? to_wgpu_string_view(label) : "Unnamed Buffer"_wgpu;

    WGPUBuffer buffer = wgpuDeviceCreateBuffer(device, &desc);

    if (data) {
        wgpuQueueWriteBuffer(queue, buffer, 0, data, size);
    }

    return buffer;
}

UniformBuffer Buffer::create_uniform_buffer(WebGPUContext& ctx, const void* data, u64 size, u32 binding, const char* label) {
    WGPUDevice device = ctx.get_native_device();
    WGPUQueue queue = ctx.get_queue()->get_native_queue();

    UniformBuffer ub;
    ub.size = size;
    ub.binding = binding;
    ub.label = label;
    ub.buffer = Buffer::create(
        device, 
        queue, 
        data, 
        size, 
        WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst, 
        label
    );

    return ub;
}

VertexBuffer Buffer::create_vertex_buffer(WebGPUContext& ctx, const void* data, u64 size, u32 slot, const char* label) {
    WGPUDevice device = ctx.get_native_device();
    WGPUQueue queue = ctx.get_queue()->get_native_queue();

    VertexBuffer vb;
    vb.size = size;
    vb.slot = slot;
    vb.label = label;
    vb.buffer = Buffer::create(
        device, 
        queue, 
        data, 
        size, 
        WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst, 
        label
    );

    return vb;
}

IndexBuffer Buffer::create_index_buffer(WebGPUContext& ctx, const void* data, u64 size, WGPUIndexFormat format, const char* label) {
    WGPUDevice device = ctx.get_native_device();
    WGPUQueue queue = ctx.get_queue()->get_native_queue();

    IndexBuffer ib;
    ib.size = size;
    ib.format = format;
    ib.label = label;
    ib.buffer = Buffer::create(
        device, 
        queue, 
        data, 
        size, 
        WGPUBufferUsage_Index | WGPUBufferUsage_CopyDst, 
        label
    );

    return ib;
}


void Buffer::example(WGPUInstance instance, WGPUDevice device, WGPUQueue queue) {
    WGPUBufferDescriptor buffer_desc = WGPU_BUFFER_DESCRIPTOR_INIT;
    buffer_desc.nextInChain = nullptr;
    buffer_desc.label = "Example Buffer"_wgpu;
    buffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_CopySrc;
    buffer_desc.size = 16;
    buffer_desc.mappedAtCreation = false;
    
    WGPUBuffer buffer1 = wgpuDeviceCreateBuffer(device, &buffer_desc);

    buffer_desc.label = "Example Buffer 2"_wgpu;
    buffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_MapRead;
    WGPUBuffer buffer2 = wgpuDeviceCreateBuffer(device, &buffer_desc);

    std::vector<uint8_t> numbers(16);
    for (uint8_t i = 0; i < 16; ++i) numbers[i] = i;

    wgpuQueueWriteBuffer(queue, buffer1, 0, numbers.data(), numbers.size());

    WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device, nullptr);

    wgpuCommandEncoderCopyBufferToBuffer(encoder, buffer1, 0, buffer2, 0, 16);

    WGPUCommandBuffer command = wgpuCommandEncoderFinish(encoder, nullptr);
    wgpuCommandEncoderRelease(encoder);
    wgpuQueueSubmit(queue, 1, &command);
    wgpuCommandBufferRelease(command);

    fetch_buffer_data_sync(
        instance,  // assumes you have a helper or store instance
        buffer2,
        [](const void* data) {
            auto* mapped = static_cast<const uint8_t*>(data);
            TR_CORE_INFO("Mapped Buffer Data:");
            for (size_t i = 0; i < 16; ++i) {
                TR_CORE_INFO("  [{}] = {}", i, static_cast<i32>(mapped[i]));
            }
        }
    );

    wgpuBufferRelease(buffer1);
    wgpuBufferRelease(buffer2);

}

}
