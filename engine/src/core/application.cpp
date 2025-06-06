#include "terra/core/application.h"
#include "terra/core/assert.h"
#include "terra/renderer/renderer.h"

// #include <GLFW/glfw3.h>

namespace terra {

Application* Application::s_instance = nullptr;
GLFWwindow* Application::s_window = nullptr;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    TR_CORE_WARN("Framebuffer size changed: {}x{}", width, height);
    glViewport(0, 0, width, height);
}


Application::Application(const std::string& name, CommandLineArgs args)
    : m_command_line_args(args), m_app_name(name)
{
    TR_CORE_ASSERT(!s_instance, "Application already exists!");
    s_instance = this;
}

Application::~Application() {
    TR_CORE_INFO("Shutting down Terra Engine...");

    if (s_window) {
        glfwDestroyWindow(s_window);
        glfwTerminate();
        s_window = nullptr;
    }

    s_instance = nullptr;
}

void Application::init() {
    TR_CORE_INFO("Initializing Terra Engine...");

    if (!glfwInit()) {
        TR_CORE_CRITICAL("Failed to initialize GLFW");
        std::exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef TR_PLATFORM_MACOS
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    s_window = glfwCreateWindow(800, 600, m_app_name.c_str(), nullptr, nullptr);
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

    
    int fbWidth, fbHeight;
    glfwGetFramebufferSize(s_window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);
    
    TR_CORE_INFO("OpenGL Initialized: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

    glfwSetFramebufferSizeCallback(s_window, framebuffer_size_callback);  

}

void Application::run() {
    Renderer::init();

    on_init();

    // render loop
    // -----------
    while (!glfwWindowShouldClose(s_window)) {
        on_update();


        Renderer::begin_frame();

        on_render();

        Renderer::draw();
        Renderer::end_frame();


        glfwSwapBuffers(s_window);
        glfwPollEvents();
    }

    on_shutdown();
    Renderer::shutdown();
}

}
