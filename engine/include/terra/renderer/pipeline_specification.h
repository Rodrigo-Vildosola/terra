#pragma once

#include "terrapch.h"
#include <webgpu/webgpu.hpp>

namespace terra {

struct VertexAttributeSpec {
    u64 offset = 0;
    WGPUVertexFormat format = WGPUVertexFormat_Float32x3;
    u32 shader_location = 0;
};

struct VertexBufferLayoutSpec {
    u64 stride = 0;
    WGPUVertexStepMode step_mode = WGPUVertexStepMode_Vertex;
    std::vector<VertexAttributeSpec> attributes;
};

struct PipelineSpecification {
    std::string_view shader_path;                // WGSL file (contains both vs/fs)
    std::string_view vertex_entry = "vs_main";   // Entry function in WGSL
    std::string_view fragment_entry = "fs_main"; // Entry function in WGSL

    WGPUTextureFormat surface_format = WGPUTextureFormat_BGRA8Unorm;
    uint64_t uniform_buffer_size = sizeof(float) * 4;

    bool enable_depth = false;
    WGPUCompareFunction depth_compare = WGPUCompareFunction_Less;
    WGPUTextureFormat depth_format = WGPUTextureFormat_Depth24Plus;

    std::vector<VertexBufferLayoutSpec> vertex_buffers;
};

} // namespace terra
