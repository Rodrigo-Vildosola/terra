#include "terra/core/application.h"
#include "terrapch.h"

namespace terra {

Application* Application::s_instance = nullptr;

Application::Application(const std::string& name, CommandLineArgs args)
    : m_command_line_args(args)
{
    TR_CORE_ASSERT(!s_instance, "Application already exists!");
    s_instance = this;
}

Application::~Application() {
    s_instance = nullptr;
}

void Application::run() {
    on_init();

    while (m_running) {
        on_update();
        on_render();
        // TODO: time step, events, etc.
    }

    on_shutdown();
}

}
