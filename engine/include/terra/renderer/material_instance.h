#pragma once

#include "terrapch.h"

#include <webgpu/webgpu.hpp>
#include "terra/renderer/pipeline.h"
#include "terra/renderer/buffer.h"


namespace terra {


class MaterialInstance {
public:
    MaterialInstance(WebGPUContext& context, Pipeline* pipeline);

    void set_uniform_data(u32 binding_index, const void* data, size_t size);
    void bind(WGPURenderPassEncoder pass_encoder);
    WGPUBindGroup get_bind_group(u32 index = 0) const;

private:
    WebGPUContext& m_context;
    Pipeline* m_pipeline;

    std::vector<UniformBuffer> m_uniforms;
    WGPUBindGroup m_bind_group = nullptr;

    void create_uniform_buffers();
    void create_bind_group();
};



}
