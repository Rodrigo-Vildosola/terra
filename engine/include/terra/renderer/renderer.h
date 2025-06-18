#pragma once

#include "terra/core/base.h"
#include "terra/renderer/buffer.h"
#include "terra/renderer/pipeline.h"

namespace terra {

class WebGPUContext;  // forward
class CommandQueue;   // forward


class Renderer {
public:
    explicit Renderer(WebGPUContext& context);
    ~Renderer();

    void init();                 // allocate pipelines, resources, etc.
    // void begin_frame();          // acquire surface, start pass
    // void end_frame();            // end pass, submit, present

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
    void clear_color(float r, float g, float b, float a);

    void draw();


    WGPURenderPassEncoder get_current_pass_encoder() const { return m_current_pass; }


private:
    WebGPUContext&   m_context;
    CommandQueue&    m_queue;

    // holds the surface‐texture alive until present()
    WGPUSurfaceTexture m_surface_texture{};
    WGPUTextureView    m_target_view{};

    scope<Pipeline> m_pipeline;
    WGPUBuffer m_vertex_buffer = nullptr;
    u32 m_vertex_count = 0;

    // last-open pass encoder
    WGPURenderPassEncoder m_current_pass = nullptr;

};

}
