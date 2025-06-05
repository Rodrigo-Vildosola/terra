#include "terra/core/application.h"
#include "terra/core/assert.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

namespace terra {

Application* Application::s_instance = nullptr;
static GLFWwindow* s_window = nullptr;

Application::Application(const std::string& name, CommandLineArgs args)
    : m_command_line_args(args)
{
    // TR_CORE_ASSERT(!s_instance, "Application already exists!");
    s_instance = this;

    TR_CORE_INFO("Initializing Terra Engine...");

    if (!glfwInit()) {
        TR_CORE_CRITICAL("Failed to initialize GLFW");
        std::exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    s_window = glfwCreateWindow(800, 600, name.c_str(), nullptr, nullptr);
    if (!s_window) {
        TR_CORE_CRITICAL("Failed to create GLFW window");
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(s_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        TR_CORE_CRITICAL("Failed to initialize GLAD");
        std::exit(EXIT_FAILURE);
    }

    TR_CORE_INFO("OpenGL Initialized: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
}

Application::~Application() {
    TR_CORE_INFO("Shutting down Terra Engine...");

    glfwDestroyWindow(s_window);
    glfwTerminate();

    s_instance = nullptr;
}

void Application::run() {
    on_init();

    while (!glfwWindowShouldClose(s_window)) {
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        on_update();
        on_render();

        glfwSwapBuffers(s_window);
        glfwPollEvents();
    }

    on_shutdown();
}

}
