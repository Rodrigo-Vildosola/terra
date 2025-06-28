#include "layer.h"
#include "terra/core/application.h"
#include "terra/renderer/renderer_api.h"
#include "terra/renderer/orthographic_camera.h"
#include "terra/renderer/perspective_camera.h"

#include "terra/resources/resource_manager.h"
#include "terra/core/timestep.h"
#include "terra/core/timer.h"

#include <imgui.h> // if you're using ImGui (optional)
#include <glm/gtc/type_ptr.hpp>

struct UniformBlock {
    glm::mat4 u_view_proj;
    glm::mat4 u_model;
    float u_time;
    float u_aspect_ratio;
    float _padding1[2]; // pad to 16-byte alignment
    glm::vec4 u_color;
};

ExampleLayer::ExampleLayer()
    : Layer("ExampleLayer") {}

void ExampleLayer::on_attach() {
    TR_INFO("ExampleLayer attached");

    float width = terra::Application::get().get_window().get_width();
    float height = terra::Application::get().get_window().get_height();
    float aspect_ratio = width / height;


    // m_camera = terra::create_scope<terra::OrthographicCamera>(-1.6f, 1.6f, -0.9f, 0.9f);
    m_camera = terra::create_scope<terra::PerspectiveCamera>(
        45.0f,                      // Field of view in degrees
        aspect_ratio,              // width / height
        0.1f,                      // Near plane
        100.0f                     // Far plane
    );
    
    m_mesh = terra::Mesh::from_file("objects/pyramid.txt");

    m_shader = terra::RendererAPI::create_shader("shaders/shader.wgsl", "Triangle Shader Module");
    m_shader->vertex_entry = "vs_main";
    m_shader->fragment_entry = "fs_main";

    m_material = terra::RendererAPI::create_material("BasicMaterial", m_shader);
    m_material->define_parameter("ubo", 0, terra::MaterialParamType::Custom, WGPUShaderStage_Vertex | WGPUShaderStage_Fragment);


    terra::PipelineSpecification spec;
    spec.shader = m_shader;
    spec.surface_format = terra::RendererAPI::get_context().get_preferred_format();

    terra::VertexBufferLayoutSpec vb;
    vb.stride = sizeof(terra::Vertex);
    vb.step_mode = WGPUVertexStepMode_Vertex;
    vb.attributes = {
        { 0, WGPUVertexFormat_Float32x3, offsetof(terra::Vertex, position) },
        { 1, WGPUVertexFormat_Float32x3, offsetof(terra::Vertex, color) },
    };
    spec.vertex_buffers.push_back(vb);

    // static_assert(sizeof(terra::Vertex) == 20, "Vertex struct size mismatch!");
    // TR_INFO("Vertex offsets: position = {}, color = {}", offsetof(terra::Vertex, position), offsetof(terra::Vertex, color));


    terra::UniformSpec ubo_spec;
    ubo_spec.binding = 0;
    ubo_spec.size = sizeof(UniformBlock);
    ubo_spec.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;

    spec.uniforms.clear();
    spec.uniforms.push_back(ubo_spec);

    m_pipeline = terra::create_scope<terra::Pipeline>(terra::RendererAPI::get_context(), spec);

    m_material_instance = m_material->create_instance(m_pipeline.get());
}

void ExampleLayer::on_detach() {
    TR_INFO("ExampleLayer detached");
}

float my_color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };

void ExampleLayer::on_update(terra::Timestep ts) {
    float width = (float)terra::Application::get().get_window().get_width();
    float height = (float)terra::Application::get().get_window().get_height();
    float aspect_ratio = width / height;

    // float time = terra::Timer::elapsed();

    UniformBlock block;
    block.u_view_proj = m_camera->get_view_matrix() * m_camera->get_projection_matrix();
    block.u_model =  m_mesh->get_model_matrix();
    block.u_time = m_cycle;
    block.u_aspect_ratio = aspect_ratio;
    block.u_color = glm::vec4(my_color[0], my_color[1], my_color[2], my_color[3]);

    m_material_instance->set_parameter("ubo", &block, sizeof(UniformBlock));


    terra::RendererAPI::clear_color(0.1f, 0.1f, 0.1f, 1.0f);
    terra::RendererAPI::begin_scene(*m_camera);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.5f));


    terra::RendererAPI::submit(m_mesh, m_material_instance, model);
    
    terra::RendererAPI::end_scene();

    m_ui_stats_timer += ts.get_seconds();

    if (m_ui_stats_timer >= 0.16f) {
        const auto& stats = terra::RendererAPI::get_stats();
        m_displayed_fps = stats.fps;
        m_displayed_frame_time = stats.frame_time_ms;
        m_ui_stats_timer = 0.0f;
    }

}

void ExampleLayer::on_physics_update(terra::Timestep fixed_ts) {

}


void ExampleLayer::on_ui_render() {

    {
        ImGui::Begin("Debug");
        glm::vec3 cam_pos = m_camera->get_position();
        glm::vec2 cam_rot = m_camera->get_rotation(); // pitch, yaw

        if (ImGui::DragFloat3("Camera Position", glm::value_ptr(cam_pos), 0.1f)) {
            m_camera->set_position(cam_pos);
        }
        if (ImGui::DragFloat2("Camera Rotation (Pitch/Yaw)", glm::value_ptr(cam_rot), 1.0f)) {
            m_camera->set_rotation(cam_rot.x, cam_rot.y);
        }

        ImGui::DragFloat("Time", &m_cycle, 0.01f);
        ImGui::ColorEdit4("Color", my_color);
        ImGui::End();
    }

    {
        const auto& stats = terra::RendererAPI::get_stats();

        ImGui::Begin("Renderer Stats");
        ImGui::Text("FPS: %.1f", m_displayed_fps);
        ImGui::Text("Frame Time: %.2f ms", m_displayed_frame_time);
        ImGui::Separator();
        ImGui::Text("Draw Calls: %u", stats.draw_calls);
        ImGui::Text("Mesh Count: %u", stats.mesh_count);
        ImGui::Text("Vertex Count: %u", stats.vertex_count);
        ImGui::Text("Index Count: %u", stats.index_count);
        ImGui::End();
    }

}

void ExampleLayer::on_event(terra::Event& event) {
    using namespace terra;

    // Handle keyboard events
    if (event.is_in_category(EventCategoryKeyboard)) {
        TR_TRACE("ExampleLayer received keyboard event: {}", event.to_string());

        // Only handle KeyPressedEvent
        if (event.get_event_type() == EventType::KeyPressed) {
            auto& key_event = static_cast<KeyPressedEvent&>(event);
            if (key_event.get_key_code() == Key::Q) {
                TR_INFO("Q key pressed, closing application...");
                Application::get().close();  // Triggers m_running = false
                event.handled = true;
            }
        }
    }

    // Handle mouse events (optional)
    if (event.is_in_category(EventCategoryMouse)) {
        // TR_TRACE("ExampleLayer received mouse event: {}", event.to_string());
    }
}
