#pragma once

#include "terrapch.h"
#include "terra/renderer/shader.h"

namespace terra {

struct VertexAttributeSpec {
    u32 shader_location = 0;
    WGPUVertexFormat format = WGPUVertexFormat_Float32x3;
    u64 offset = 0;
};

struct VertexBufferLayoutSpec {
    u64 stride = 0;
    WGPUVertexStepMode step_mode = WGPUVertexStepMode_Vertex;
    std::vector<VertexAttributeSpec> attributes;
};

struct PipelineSpecification {
    PipelineSpecification() = delete;
    PipelineSpecification(Shader&& shader)
        : shader(std::move(shader)) {}

    PipelineSpecification(PipelineSpecification&&) = default;
    PipelineSpecification& operator=(PipelineSpecification&&) = default;

    PipelineSpecification(const PipelineSpecification&) = delete;
    PipelineSpecification& operator=(const PipelineSpecification&) = delete;


    Shader shader;

    WGPUTextureFormat surface_format = WGPUTextureFormat_BGRA8Unorm;
    u64 uniform_buffer_size = sizeof(f32) * 4;

    bool enable_depth = false;
    WGPUCompareFunction depth_compare = WGPUCompareFunction_Less;
    WGPUTextureFormat depth_format = WGPUTextureFormat_Depth24Plus;

    std::vector<VertexBufferLayoutSpec> vertex_buffers;
};

} // namespace terra
