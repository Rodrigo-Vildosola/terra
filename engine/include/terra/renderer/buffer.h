#pragma once

#include "terra/core/context/context.h"
#include "terrapch.h"
#include <webgpu/webgpu.hpp>


namespace terra {

struct UniformBuffer {
    WGPUBuffer buffer = nullptr;
    u64 size = 0;
    u32 binding = 0;
    const char* label = "Uniform Buffer";
};

struct VertexBuffer {
    WGPUBuffer buffer = nullptr;
    u64 size = 0;
    u32 slot = 0; // Index to bind the vertex buffer (setVertexBuffer)
    const char* label = "Vertex Buffer";
};

struct IndexBuffer {
    WGPUBuffer buffer = nullptr;
    u64 size = 0;
    WGPUIndexFormat format = WGPUIndexFormat_Uint32;
    const char* label = "Index Buffer";
};

struct StorageBuffer {
    WGPUBuffer buffer = nullptr;
    u64 size = 0;
    u32 binding = 0;
    const char* label = "Storage Buffer";
};


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

    static UniformBuffer create_uniform_buffer(
        WebGPUContext& ctx,
        const void* data,
        u64 size,
        u32 binding,
        const char* label = "Uniform Buffer"
    );

    static VertexBuffer create_vertex_buffer(
        WebGPUContext& ctx,
        const void* data,
        u64 size,
        u32 slot = 0,
        const char* label = "Vertex Buffer"
    );

    static IndexBuffer create_index_buffer(
        WebGPUContext& ctx,
        const void* data,
        u64 size,
        WGPUIndexFormat format = WGPUIndexFormat_Uint32,
        const char* label = "Index Buffer"
    );

    static StorageBuffer create_storage_buffer(
        WebGPUContext& ctx,
        const void* data,
        u64 size,
        u32 binding,
        const char* label = "Storage Buffer"
    );
};

void fetch_buffer_data_sync(WGPUInstance instance, WGPUBuffer buffer, std::function<void(const void*)> process_buffer_data);

}
