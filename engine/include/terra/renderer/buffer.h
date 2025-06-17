#pragma once

#include <webgpu/webgpu.hpp>


namespace terra {

class Buffer {
public:
    static void example(WGPUInstance instance, WGPUDevice device, WGPUQueue queue);


    static WGPUBuffer create(
        WGPUDevice device,
        WGPUQueue queue,
        const void* data,
        size_t size,
        WGPUBufferUsage usage,
        const char* label = "Unnamed Buffer"
    );
};

void fetch_buffer_data_sync(WGPUInstance instance, WGPUBuffer buffer, std::function<void(const void*)> process_buffer_data);

}
