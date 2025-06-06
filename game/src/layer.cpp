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

void ExampleLayer::on_ui_render() {
    // TR_INFO("ImGui render triggered"); // Add this temporarily

    // Optional: ImGui or other UI rendering
    ImGui::Begin("Example UI");
    ImGui::Text("Hello from ExampleLayer");
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
