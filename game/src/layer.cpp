#include "layer.h"

#include <imgui.h> // if you're using ImGui (optional)

ExampleLayer::ExampleLayer()
    : Layer("ExampleLayer") {}

void ExampleLayer::on_attach() {
    TR_INFO("ExampleLayer attached");
    // Initialize resources, load textures, setup state...
}

void ExampleLayer::on_detach() {
    TR_INFO("ExampleLayer detached");
    // Cleanup if needed
}

void ExampleLayer::on_update(terra::Timestep ts) {
    // Called every frame with delta time
    // TR_TRACE("ExampleLayer updating: {}s", ts.get_seconds());

    // Example: move something, run animations, update logic
}
bool my_tool_active = true;
float my_color[4] = { 1.0f, 0.0f, 0.0f, 1.0f };

void ExampleLayer::on_ui_render() {

    // ImGui::NewFrame();

    // TR_INFO("ImGui render triggered"); // Add this temporarily
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
