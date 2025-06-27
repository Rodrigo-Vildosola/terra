// terra_format.h
#pragma once
#include <webgpu/webgpu.h>


namespace terra {

enum class ShaderStage {
    None     = 0,
    Vertex   = 1 << 0,
    Fragment = 1 << 1,
    All      = Vertex | Fragment
};

enum class VertexFormat {
    Float32x2,
    Float32x3,
    Float32x4,
    Uint32,
    // Add more as needed
};

enum class VertexStepMode {
    Vertex,
    Instance
};

inline WGPUVertexFormat to_wgpu(VertexFormat fmt) {
    switch (fmt) {
        case VertexFormat::Float32x2: return WGPUVertexFormat_Float32x2;
        case VertexFormat::Float32x3: return WGPUVertexFormat_Float32x3;
        case VertexFormat::Float32x4: return WGPUVertexFormat_Float32x4;
        case VertexFormat::Uint32:    return WGPUVertexFormat_Uint32;
        default:                      return WGPUVertexFormat_Uint32;
    }
}

inline WGPUVertexStepMode to_wgpu(VertexStepMode step) {
    return step == VertexStepMode::Vertex
        ? WGPUVertexStepMode_Vertex
        : WGPUVertexStepMode_Instance;
}

inline WGPUShaderStage to_wgpu(ShaderStage stage) {
    WGPUShaderStage flags = 0;
    if ((int)stage & (int)ShaderStage::Vertex)   flags |= WGPUShaderStage_Vertex;
    if ((int)stage & (int)ShaderStage::Fragment) flags |= WGPUShaderStage_Fragment;
    return flags;
}

} // namespace terra
