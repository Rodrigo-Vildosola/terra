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

struct UniformSpec {
    u32 binding;
    u64 size;
    WGPUShaderStage visibility;
};

struct PipelineSpecification {
    ref<Shader> shader = nullptr;

    WGPUTextureFormat surface_format;
    std::vector<VertexBufferLayoutSpec> vertex_buffers;

    std::vector<UniformSpec> uniforms;

    WGPUTextureView depth_view;

};

} // namespace terra
