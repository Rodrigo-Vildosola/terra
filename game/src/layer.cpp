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

    m_camera = terra::create_scope<terra::PerspectiveCamera>(
        45.0f,                        // Field of view in degrees
        aspect_ratio,               // width / height
        0.1f,                 // Near plane
        100.0f                   // Far plane
    );
    
    m_mesh = terra::Mesh::from_file("objects/pyramid.txt");
    m_mesh_2 = terra::Mesh::from_file("objects/webgpu.txt");


    m_shader = terra::RendererAPI::create_shader("shaders/shader.wgsl", "Triangle Shader Module");
    m_shader->vertex_entry = "vs_main";
    m_shader->fragment_entry = "fs_main";

    m_material = terra::RendererAPI::create_material("BasicMaterial", m_shader);
    m_material->define_parameter("ubo", 0, terra::MaterialParamType::Custom, wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment);


    terra::PipelineSpecification spec;
    spec.shader = m_shader;

    terra::VertexBufferLayoutSpec vb;
    vb.stride = sizeof(terra::Vertex);
    vb.step_mode = wgpu::VertexStepMode::Vertex;
    vb.attributes = {
        { 0, wgpu::VertexFormat::Float32x3, offsetof(terra::Vertex, position) },
        { 1, wgpu::VertexFormat::Float32x3, offsetof(terra::Vertex, color) },
    };
    spec.vertex_buffers.push_back(vb);

    terra::UniformBufferSpec ubo_spec;
    ubo_spec.binding = 0;
    ubo_spec.size = sizeof(UniformBlock);
    ubo_spec.visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;

    spec.uniforms = { ubo_spec };

    terra::StorageBufferSpec sb_spec;
    sb_spec.binding = 0;
    sb_spec.size = sizeof(InstanceBlock);
    sb_spec.visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;

    spec.storages = { sb_spec };

    terra::u64 pipeline_id = terra::RendererAPI::create_pipeline(spec);

    auto pipeline = terra::RendererAPI::get_pipeline(pipeline_id);

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

        TR_CORE_INFO("FPS: {:.1f} | Frame Time: {:.2f} ms", m_displayed_fps, m_displayed_frame_time);

        m_fps_accumulator = 0.0f;
        m_fps_frame_count = 0;
    }

    glm::vec3 movement(0.0f);

    if (m_keys[terra::Key::W]) movement.z += 1.0f;
    if (m_keys[terra::Key::S]) movement.z -= 1.0f;
    if (m_keys[terra::Key::A]) movement.x -= 1.0f;
    if (m_keys[terra::Key::D]) movement.x += 1.0f;
    if (m_keys[terra::Key::E]) movement.y += 1.0f;
    if (m_keys[terra::Key::Q]) movement.y -= 1.0f;

    if (glm::length(movement) > 0.0f)
        movement = glm::normalize(movement);

    float dt = ts.get_seconds();
    glm::vec3 offset = movement * m_camera_speed * dt;

    // Move camera in local space
    m_camera->move(offset);


    // float time = terra::Timer::elapsed();

    UniformBlock block;
    block.u_view = m_camera->get_view_matrix();
    block.u_proj = m_camera->get_projection_matrix();
    block.u_time = ts.get_milliseconds();

    m_material_instance->set_parameter("ubo", &block, sizeof(UniformBlock));

    // ─── submit instance #1 ───
    {
        InstanceBlock I1;
        I1.model = glm::translate(glm::mat4(1.0f), I1_pos);
        I1.color = I1_color;
        terra::RendererAPI::submit(m_mesh, m_material_instance, I1, /*binding=*/0, /*group=*/1);
    }

    // ─── submit instance #2 ───
    {
        InstanceBlock I2;
        I2.model = glm::translate(glm::mat4(1.0f), I2_pos);
        I2.color = I2_color;
        terra::RendererAPI::submit(m_mesh, m_material_instance, I2, 0, 1);
    }

    {
        InstanceBlock I2;
        I2.model = glm::translate(glm::mat4(1.0f), I2_pos);
        I2.color = I2_color;
        terra::RendererAPI::submit(m_mesh_2, m_material_instance, I2, 0, 1);
    }
    
    terra::RendererAPI::end_scene();

    if (m_mouse_dragging) {
        terra::Application& app = terra::Application::get();
        glm::vec2 mouse_pos = app.get_window().get_mouse_position();

        glm::vec2 delta = mouse_pos - m_last_mouse_position;
        m_last_mouse_position = mouse_pos;

        float yaw   = delta.x * m_mouse_sensitivity;
        float pitch = -delta.y * m_mouse_sensitivity;

        glm::vec2 rot = m_camera->get_rotation();
        rot.x += pitch;
        rot.y += yaw;
        m_camera->set_rotation(rot.x, rot.y);
    }


}

void ExampleLayer::on_physics_update(terra::Timestep fixed_ts) {

}


void ExampleLayer::on_ui_render() {

    {
        ImGui::Begin("Instance Controls");

        // Instance #1 controls
        ImGui::DragFloat3("Inst1 Position", glm::value_ptr(I1_pos),   0.05f);
        ImGui::ColorEdit4("Inst1 Color",    glm::value_ptr(I1_color));

        ImGui::Separator();

        // Instance #2 controls
        ImGui::DragFloat3("Inst2 Position", glm::value_ptr(I2_pos),   0.05f);
        ImGui::ColorEdit4("Inst2 Color",    glm::value_ptr(I2_color));

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

    ImGui::Begin("Camera Controls");

    // Position
    {
        glm::vec3 pos = m_camera->get_position();
        if (ImGui::DragFloat3("Position", glm::value_ptr(pos), 0.1f)) {
            m_camera->set_position(pos);
        }
    }

    // Rotation
    {
        glm::vec2 rot = m_camera->get_rotation(); // pitch, yaw
        if (ImGui::DragFloat2("Rotation (Pitch/Yaw)", glm::value_ptr(rot), 1.0f)) {
            m_camera->set_rotation(rot.x, rot.y);
        }
    }

    // Projection params
    {
        float fov = m_camera->get_fov();
        if (ImGui::DragFloat("FOV", &fov, 0.5f, 1.0f, 179.0f)) {
            m_camera->set_fov(fov);
        }

        float aspect = m_camera->get_aspect_ratio();
        // If you want to allow forcing aspect manually:
        if (ImGui::DragFloat("Aspect Ratio", &aspect, 0.01f, 0.1f, 10.0f)) {
            m_camera->set_aspect_ratio(aspect);
        }

        float nearp = m_camera->get_near_plane();
        if (ImGui::DragFloat("Near Plane", &nearp, 0.01f, 0.01f, m_camera->get_far_plane() - 0.01f)) {
            m_camera->set_near_plane(nearp);
        }

        float farp = m_camera->get_far_plane();
        if (ImGui::DragFloat("Far Plane", &farp, 0.1f, m_camera->get_near_plane() + 0.1f, 1000.0f)) {
            m_camera->set_far_plane(farp);
        }
    }

    ImGui::End();


}

void ExampleLayer::on_event(terra::Event& event) {
    using namespace terra;

    EventDispatcher dispatcher(event);


    // Handle keyboard events
    if (event.is_in_category(EventCategoryKeyboard)) {
        TR_TRACE("ExampleLayer received keyboard event: {}", event.to_string());

        // Only handle KeyPressedEvent
        if (event.get_event_type() == EventType::KeyPressed) {
            auto& key_event = static_cast<KeyPressedEvent&>(event);
            // if (key_event.get_key_code() == Key::Q) {
            //     TR_INFO("Q key pressed, closing application...");
            //     Application::get().close();  // Triggers m_running = false
            //     event.handled = true;
            // }
        }
    }

    dispatcher.dispatch<KeyPressedEvent>([&](KeyPressedEvent& e) {
        m_keys[e.get_key_code()] = true;
        return false;
    });

    dispatcher.dispatch<KeyReleasedEvent>([&](KeyReleasedEvent& e) {
        m_keys[e.get_key_code()] = false;
        return false;
    });

    dispatcher.dispatch<MouseButtonPressedEvent>([&](MouseButtonPressedEvent& e) {
        if (e.get_mouse_button() == Mouse::ButtonRight) {
            m_mouse_dragging = true;
            m_last_mouse_position = terra::Application::get().get_window().get_mouse_position();
        }
        return false;
    });

    dispatcher.dispatch<MouseButtonReleasedEvent>([&](MouseButtonReleasedEvent& e) {
        if (e.get_mouse_button() == Mouse::ButtonRight) {
            m_mouse_dragging = false;
        }
        return false;
    });
}
