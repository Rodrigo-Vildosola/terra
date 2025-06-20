#pragma once

#include "terrapch.h"
#include "terra/renderer/pipeline_specification.h"
#include "terra/renderer/shader.h"

namespace terra {

class WebGPUContext;

class Pipeline {
public:
    Pipeline(WebGPUContext& context, PipelineSpecification& spec);
    ~Pipeline();

    void bind(WGPURenderPassEncoder encoder) const;

    WGPUBindGroupLayout get_bind_group_layout() const { return m_bind_group_layout; }

private:
    void create_pipeline(PipelineSpecification& spec); // internal

    WebGPUContext& m_context;

    WGPURenderPipeline m_pipeline;
    WGPUPipelineLayout m_layout;
    WGPUBindGroupLayout m_bind_group_layout;
};

    
} // namespace terra
