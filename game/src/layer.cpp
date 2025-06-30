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

struct alignas(16) UniformBlock {
    glm::mat4 u_view;
    glm::mat4 u_proj;
    float u_time;
    float u_aspect_ratio;
};

struct alignas(16) InstanceBlock {
    glm::mat4 model;   // 64 bytes
    glm::vec4 color;   // 16 bytes
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
    m_mesh_2 = terra::Mesh::from_file("objects/webgpu.txt");


    m_shader = terra::RendererAPI::create_shader("shaders/shader.wgsl", "Triangle Shader Module");
    m_shader->vertex_entry = "vs_main";
    m_shader->fragment_entry = "fs_main";

    m_material = terra::RendererAPI::create_material("BasicMaterial", m_shader);
    m_material->define_parameter("ubo", 0, terra::MaterialParamType::Custom, WGPUShaderStage_Vertex | WGPUShaderStage_Fragment);


    terra::PipelineSpecification spec;
    spec.shader = m_shader;

    terra::VertexBufferLayoutSpec vb;
    vb.stride = sizeof(terra::Vertex);
    vb.step_mode = WGPUVertexStepMode_Vertex;
    vb.attributes = {
        { 0, WGPUVertexFormat_Float32x3, offsetof(terra::Vertex, position) },
        { 1, WGPUVertexFormat_Float32x3, offsetof(terra::Vertex, color) },
    };
    spec.vertex_buffers.push_back(vb);

    terra::UniformBufferSpec ubo_spec;
    ubo_spec.binding = 0;
    ubo_spec.size = sizeof(UniformBlock);
    ubo_spec.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;

    spec.uniforms = { ubo_spec };

    terra::StorageBufferSpec sb_spec;
    sb_spec.binding = 0;
    sb_spec.size = sizeof(InstanceBlock);
    sb_spec.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;

    spec.storages = { sb_spec };

    terra::u64 pipeline_id = terra::RendererAPI::create_pipeline(spec);

    auto pipeline = terra::RendererAPI::get_pipeline(pipeline_id); // optional, mostly internal


    m_material_instance = m_material->create_instance(pipeline.get());
}

void ExampleLayer::on_detach() {
    TR_INFO("ExampleLayer detached");
}

static glm::vec3 I1_pos   = { 0.0f, 0.0f, 0.0f };
static glm::vec4 I1_color = { 0.2f, 0.8f, 0.2f, 1.0f };

static glm::vec3 I2_pos   = {  1.5f, 0.0f, -2.5f };
static glm::vec4 I2_color = { 0.2f, 0.2f, 1.0f, 1.0f };

void ExampleLayer::on_update(terra::Timestep ts) {

    terra::RendererAPI::clear_color(0.1f, 0.1f, 0.1f, 1.0f);
    terra::RendererAPI::begin_scene(*m_camera);

    float delta_time = ts.get_seconds();
    m_fps_accumulator += delta_time;
    m_fps_frame_count++;

    if (m_fps_accumulator >= 1.0f) {
        m_displayed_fps = (float)m_fps_frame_count / m_fps_accumulator;
        m_displayed_frame_time = 1000.0f / m_displayed_fps;  // in milliseconds

        m_fps_accumulator = 0.0f;
        m_fps_frame_count = 0;
    }

    float width = (float)terra::Application::get().get_window().get_width();
    float height = (float)terra::Application::get().get_window().get_height();
    float aspect_ratio = width / height;

    // float time = terra::Timer::elapsed();

    UniformBlock block;
    block.u_view = m_camera->get_view_matrix();
    block.u_proj = m_camera->get_projection_matrix();
    block.u_time = m_cycle;
    block.u_aspect_ratio = aspect_ratio;

    m_material_instance->set_parameter("ubo", &block, sizeof(UniformBlock));

    // ─── submit instance #1 ───
    {
        InstanceBlock I1;
        I1.model = glm::translate(glm::mat4(1.0f), I1_pos);
        I1.color = I1_color;
        terra::RendererAPI::submit(
            m_mesh, m_material_instance, &I1,
            sizeof(InstanceBlock), /*binding=*/0, /*group=*/1
        );
    }

    // ─── submit instance #2 ───
    {
        InstanceBlock I2;
        I2.model = glm::translate(glm::mat4(1.0f), I2_pos);
        I2.color = I2_color;
        terra::RendererAPI::submit(
            m_mesh, m_material_instance, &I2,
            sizeof(InstanceBlock), 0, 1
        );
    }

    {
        InstanceBlock I2;
        I2.model = glm::translate(glm::mat4(1.0f), I2_pos);
        I2.color = I2_color;
        terra::RendererAPI::submit(
            m_mesh_2, m_material_instance, &I2,
            sizeof(InstanceBlock), 0, 1
        );
    }
    
    terra::RendererAPI::end_scene();

}

void ExampleLayer::on_physics_update(terra::Timestep fixed_ts) {

}


void ExampleLayer::on_ui_render() {

    {
        ImGui::Begin("Instance Controls");

        // Instance #1 controls
        ImGui::DragFloat3("Inst1 Position", glm::value_ptr(I1_pos),   0.05f);
        ImGui::ColorEdit4("Inst1 Color",    glm::value_ptr(I1_color));

        // Instance #2 controls
        ImGui::DragFloat3("Inst2 Position", glm::value_ptr(I2_pos),   0.05f);
        ImGui::ColorEdit4("Inst2 Color",    glm::value_ptr(I2_color));

        ImGui::DragFloat("Time", &m_cycle, 0.01f);
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
