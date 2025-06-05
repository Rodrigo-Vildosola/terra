#pragma once

#include "terra/core/logger.h"
#include <string>

namespace terra {

struct CommandLineArgs {
    int count = 0;
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

    void run();  // Main loop

    virtual void on_init() {}
    virtual void on_update() {}
    virtual void on_render() {}
    virtual void on_shutdown() {}

    CommandLineArgs get_command_line_args() const { return m_command_line_args; }

    static Application* get() { return s_instance; }

protected:
    bool m_running = true;

private:
    CommandLineArgs m_command_line_args;

    float m_last_frame_time = 0.0f;
    static Application* s_instance;
};

Application* create_application(CommandLineArgs args);  // implemented by the game

} // namespace terra
