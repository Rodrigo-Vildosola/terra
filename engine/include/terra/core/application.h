#pragma once

#include "terra/core/base.h"
#include "terra/core/logger.h"

#include "terra/core/window.h"

#include "terra/core/layer.h"
#include "terra/core/layer_stack.h"

#include "terra/events/event.h"
#include "terra/events/application_event.h"

#include "terra/ui/imgui_layer.h"

#include <string>

int main(int argc, char** argv);

namespace terra {

struct CommandLineArgs {
    i32 count = 0;
    char** args = nullptr;

    const char* operator[](int index) const {
        TR_CORE_ASSERT(index < count, "Index out of bounds for CommandLineArgs");
        return args[index];
    }
};

class Application {
public:
    Application(const std::string& name = "Terra Application", CommandLineArgs args = {});
    virtual ~Application();

    void on_event(Event& e);

    void push_layer(Layer* layer);
	void push_overlay(Layer* layer);

    CommandLineArgs get_command_line_args() const { return m_command_line_args; }

    inline Window& get_window() const { return *m_window; }
    void close();

    UILayer* get_ui_layer() { return m_ui_layer; }

    static Application& get() { return *s_instance; }


private:
    void run();

    bool on_window_close(WindowCloseEvent& e);
    bool on_window_resize(WindowResizeEvent& e);

    UILayer* m_ui_layer;
    LayerStack m_layer_stack;
    CommandLineArgs m_command_line_args;

    scope<Window> m_window;
    bool m_running = true;
	bool m_minimized = false;

    f32 m_last_frame_time = 0.0f;

    static Application* s_instance;
    friend int ::main(int argc, char** argv);
};

Application* create_application(CommandLineArgs args);  // implemented by the game

} // namespace terra
