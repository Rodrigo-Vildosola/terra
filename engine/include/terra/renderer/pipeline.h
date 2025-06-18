#pragma once

#include "terrapch.h"
#include "terra/renderer/pipeline_specification.h"
#include "terra/renderer/shader.h"

namespace terra {

class WebGPUContext;

class Pipeline {
public:
    Pipeline(WebGPUContext& context, PipelineSpecification&& spec);
    ~Pipeline();

    void bind(WGPURenderPassEncoder encoder) const;

    const PipelineSpecification& get_spec() const { return m_spec; }

private:
    void create_pipeline(); // internal

    WebGPUContext& m_context;
    PipelineSpecification m_spec;

    WGPURenderPipeline m_pipeline;
    WGPUPipelineLayout m_layout;
    WGPUBindGroupLayout m_bind_group_layout;
};

    
} // namespace terra
