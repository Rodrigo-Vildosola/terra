#pragma once

#include "terra/core/base.h"
#include "terra/renderer/mesh.h"
#include "terra/renderer/renderer.h"
#include "terra/renderer/shader.h"
#include "terra/renderer/material.h"
#include "terra/renderer/material_instance.h"
#include <glm/glm.hpp>

namespace terra {

// Forward declaration
class Camera;

class RendererAPI {
public:
    static void init(WebGPUContext* context);
    static void shutdown();

    static void begin_frame();
    static void end_frame();

    static void begin_ui_pass();
    static void end_ui_pass();

    static void clear_color(f32 r, f32 g, f32 b, f32 a);

    static ref<Shader> create_shader(const std::string& path, const std::string& label = "");
    static ref<Material> create_material(const std::string& name, const ref<Shader>& shader);
    static ref<Mesh> create_mesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices);

    static void begin_scene(const Camera& camera);
    static void end_scene();

    static void submit(const ref<Mesh>& mesh, const ref<MaterialInstance>& material_instance, const glm::mat4& transform = glm::mat4(1.0f));
    
    static WebGPUContext& get_context();
    static WGPURenderPassEncoder get_current_pass_encoder();

    static const RendererStats& get_stats();
    static RendererStats& get_stats_mutable();

private:
    struct RendererAPIData {
        WebGPUContext* context = nullptr;
    };

    static scope<RendererAPIData> s_data;

    static scope<class Renderer> s_renderer;
};

} 
