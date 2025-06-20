#pragma once

#include "terra/core/base.h"
#include "terra/core/timestep.h"
#include "terra/renderer/buffer.h"
#include "terra/renderer/pipeline.h"

namespace terra {

class WebGPUContext;  // forward declare
class CommandQueue;   // forward declare


class Renderer {
public:
    explicit Renderer(WebGPUContext& context);
    ~Renderer();

    void init();                 // allocate pipelines, resources, etc.

    void update_uniforms(f32 time);


    // call once per frame
    void begin_frame();
    void begin_scene_pass();
    void end_scene_pass();
    void begin_ui_pass();
    void end_ui_pass();
    void end_frame();

    void draw_scene();    // your 3D draw calls
    void draw_ui();       // nothing here: UI is done outside

    // high‐level draws
    void clear_color(f32 r, f32 g, f32 b, f32 a);

    void draw();


    WGPURenderPassEncoder get_current_pass_encoder() const { return m_current_pass; }


private:
    void init_bind_group();

    WebGPUContext&   m_context;
    CommandQueue&    m_queue;

    // holds the surface‐texture alive until present()
    WGPUSurfaceTexture m_surface_texture{};
    WGPUTextureView    m_target_view{};

    scope<Pipeline> m_pipeline;
    VertexBuffer m_vertex_buffer;

    IndexBuffer m_index_buffer;
    u32 m_index_count = 0;

    UniformBuffer m_uniform_buffer;
    WGPUBindGroup m_bind_group = nullptr;

    // last-open pass encoder
    WGPURenderPassEncoder m_current_pass = nullptr;

};

}
