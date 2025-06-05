#pragma once

#include "terra/core/base.h"
#include "terra/core/logger.h"
#include <string>

int main(int argc, char** argv);

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

    void init();
    void run();

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
    std::string m_app_name;
    static Application* s_instance;

    float m_last_frame_time = 0.0f;
};

Application* create_application(CommandLineArgs args);  // implemented by the game

} // namespace terra
