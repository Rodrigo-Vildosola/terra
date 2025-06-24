#pragma once

#include "terra/core/base.h"
#include "terra/core/timestep.h"
#include "terra/renderer/buffer.h"
#include "terra/renderer/pipeline.h"
#include "terra/renderer/material_instance.h"
#include "terra/renderer/camera.h"
#include "terra/renderer/mesh.h"

namespace terra {

class WebGPUContext;  // forward declare
class CommandQueue;   // forward declare


class Renderer {
public:
    explicit Renderer(WebGPUContext& context);
    ~Renderer();

    void init();

    void update_uniforms(f32 time);


    // call once per frame
    void begin_frame();
    void end_frame();
    
    void begin_scene(const Camera& camera);
    void end_scene();
    void submit(const ref<Mesh>& mesh, const ref<MaterialInstance>& material_instance, const glm::mat4& transform);

    void begin_ui_pass();
    void end_ui_pass();

    void draw_scene();
    void draw_ui();

    // high‐level draws
    void clear_color(f32 r, f32 g, f32 b, f32 a);

    void draw();


    WGPURenderPassEncoder get_current_pass_encoder() const { return m_current_pass; }


private:
    struct SceneData {
        const Camera* camera;
    };
    scope<SceneData> m_scene_data;

    WebGPUContext&   m_context;
    CommandQueue&    m_queue;

    // holds the surface‐texture alive until present()
    WGPUSurfaceTexture m_surface_texture{};
    WGPUTextureView    m_target_view{};

    WGPURenderPassEncoder m_current_pass = nullptr;

};

}
