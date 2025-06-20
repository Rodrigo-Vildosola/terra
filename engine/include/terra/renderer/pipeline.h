#pragma once

#include "terrapch.h"
#include "terra/renderer/pipeline_specification.h"
#include "terra/renderer/shader.h"

namespace terra {

class WebGPUContext;

class Pipeline {
public:
    Pipeline(WebGPUContext& context, const PipelineSpecification& spec);
    ~Pipeline();

    void bind(WGPURenderPassEncoder encoder) const;

private:
    void create_pipeline(const PipelineSpecification& spec); // internal

    WebGPUContext& m_context;

    WGPURenderPipeline m_pipeline;
    WGPUPipelineLayout m_layout;
    WGPUBindGroupLayout m_bind_group_layout;
};

    
} // namespace terra
