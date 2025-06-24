#include "layer.h"
#include "terra/renderer/renderer_api.h"
#include "terra/renderer/orthographic_camera.h"
#include "terra/resources/resource_manager.h"
#include "terra/core/timestep.h"
#include "terra/core/timer.h"

#include <imgui.h> // if you're using ImGui (optional)
#include <glm/gtc/type_ptr.hpp>

ExampleLayer::ExampleLayer()
    : Layer("ExampleLayer") {}

void ExampleLayer::on_attach() {
    TR_INFO("ExampleLayer attached");

    m_camera = terra::create_scope<terra::OrthographicCamera>(-1.6f, 1.6f, -0.9f, 0.9f);

    std::vector<float> vertex_data;
    std::vector<uint32_t> index_data;
    terra::ResourceManager::load_geometry("objects/webgpu.txt", vertex_data, index_data);

    std::vector<terra::Vertex> vertices;
    for (size_t i = 0; i < vertex_data.size(); i += 5) {
        terra::Vertex v;
        v.position = {vertex_data[i+0], vertex_data[i+1]};
        v.color = {vertex_data[i+2], vertex_data[i+3], vertex_data[i+4]};
        vertices.push_back(v);
    }
    
    m_mesh = terra::RendererAPI::create_mesh(vertices, index_data);

    m_shader = terra::RendererAPI::create_shader("shaders/shader.wgsl", "Triangle Shader Module");
    m_shader->vertex_entry = "vs_main";
    m_shader->fragment_entry = "fs_main";

    m_material = terra::RendererAPI::create_material("BasicMaterial", m_shader);
    m_material->define_parameter("u_camera", 0, terra::MaterialParamType::Matrix4x4, WGPUShaderStage_Vertex);
    m_material->define_parameter("u_time", 1, terra::MaterialParamType::Float, WGPUShaderStage_Vertex | WGPUShaderStage_Fragment);
    m_material->set_default_float("u_time", 0.0f);

    terra::PipelineSpecification spec;
    spec.shader = m_shader;
    spec.surface_format = terra::RendererAPI::get_context().get_preferred_format();

    terra::VertexBufferLayoutSpec vb;
    vb.stride = sizeof(terra::Vertex);
    vb.step_mode = WGPUVertexStepMode_Vertex;
    vb.attributes = {
        { 0, WGPUVertexFormat_Float32x2, offsetof(terra::Vertex, position) },
        { 1, WGPUVertexFormat_Float32x3, offsetof(terra::Vertex, color) },
    };
    spec.vertex_buffers.push_back(vb);

    terra::UniformSpec camera_uniform;
    camera_uniform.binding = 0;
    camera_uniform.size = sizeof(glm::mat4);
    camera_uniform.visibility = WGPUShaderStage_Vertex;
    spec.uniforms.push_back(camera_uniform);

    terra::UniformSpec time_uniform;
    time_uniform.binding = 1;
    time_uniform.size = sizeof(terra::f32);
    time_uniform.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
    spec.uniforms.push_back(time_uniform);
    
    m_pipeline = terra::create_scope<terra::Pipeline>(terra::RendererAPI::get_context(), spec);

    m_material_instance = m_material->create_instance(m_pipeline.get());
}

void ExampleLayer::on_detach() {
    TR_INFO("ExampleLayer detached");
}

void ExampleLayer::on_update(terra::Timestep ts) {
    // TR_INFO("HOLAAA");
    // float time = terra::Timer::elapsed();
    glm::mat4 view_proj = m_camera->get_projection_matrix() * m_camera->get_view_matrix();
    m_material_instance->set_parameter_matrix4x4("u_camera", glm::value_ptr(view_proj));
    m_material_instance->set_parameter_float("u_time", m_cycle);

    terra::RendererAPI::clear_color(0.1f, 0.1f, 0.1f, 1.0f);
    terra::RendererAPI::begin_scene(*m_camera);

    terra::RendererAPI::submit(m_mesh, m_material_instance, glm::mat4(1.0f));
    
    terra::RendererAPI::end_scene();
}

bool my_tool_active = true;
float my_color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };

void ExampleLayer::on_ui_render() {
    ImGui::Begin("Debug");
    glm::vec3 cam_pos = m_camera->get_position();
    float cam_rot = m_camera->get_rotation();

    if (ImGui::DragFloat3("Camera Position", glm::value_ptr(cam_pos), 0.1f)) {
        m_camera->set_position(cam_pos);
    }
    if (ImGui::DragFloat("Camera Rotation", &cam_rot, 1.0f)) {
        m_camera->set_rotation(cam_rot);
    }

    ImGui::DragFloat("Time", &m_cycle, 0.01f);
    ImGui::End();

    // Create a window called "My First Tool", with a menu bar.
    bool is_it = ImGui::Begin("My First Tool", &my_tool_active, ImGuiWindowFlags_MenuBar);
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
            if (ImGui::MenuItem("Save", "Ctrl+S"))   { /* Do stuff */ }
            if (ImGui::MenuItem("Close", "Ctrl+W"))  { my_tool_active = false; }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    // Edit a color stored as 4 floats
    ImGui::ColorEdit4("Color", my_color);

    // Generate samples and plot them
    float samples[100];
    for (int n = 0; n < 100; n++)
        samples[n] = sinf(n * 0.2f + ImGui::GetTime() * 1.5f);
    ImGui::PlotLines("Samples", samples, 100);

    // Display contents in a scrolling region
    ImGui::TextColored(ImVec4(1,1,0,1), "Important Stuff");
    ImGui::BeginChild("Scrolling");
    for (int n = 0; n < 50; n++)
        ImGui::Text("%04d: Some text", n);
    ImGui::EndChild();
    ImGui::End();
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
