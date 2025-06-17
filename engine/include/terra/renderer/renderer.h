#pragma once

#include "terra/core/base.h"
#include "terra/core/context/context.h"

#include "terra/renderer/pipeline.h"
#include "terra/core/context/command_queue.h"
#include "terra/renderer/buffer.h"

namespace terra {

class Pipeline;

class Renderer {
public:
    explicit Renderer(WebGPUContext& context);
    ~Renderer();

    void init();                 // allocate pipelines, resources, etc.
    WGPURenderPassEncoder begin_frame();          // acquire surface, start pass
    void end_frame();            // end pass, submit, present

    // high‐level draws
    void clear_color(float r, float g, float b, float a);

    void draw();


    WGPURenderPassEncoder get_render_pass_encoder();


private:
    WebGPUContext&   m_context;
    CommandQueue&    m_queue;    // grab from context
    scope<Pipeline> m_pipeline;

    WGPUBuffer m_vertex_buffer = nullptr;
    u32 m_vertex_count = 0;
};

}
