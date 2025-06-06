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

#ifdef __APPLE__
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

    TR_CORE_INFO("OpenGL Initialized: {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(s_window, framebuffer_size_callback);  

}

constexpr char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

constexpr char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0, 0.5, 0.2, 1.0);\n"
    "}\0";

void Application::run() {
    on_init();

    i32 success;
    char infoLog[512];

    // Vertex Shader
    u32 vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        TR_CORE_ERROR("Vertex Shader Compilation Failed: {0}", infoLog);
    }

    // Fragment Shader
    u32 fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        TR_CORE_ERROR("Fragment Shader Compilation Failed: {0}", infoLog);
    }

    u32 shaderProgram = glCreateProgram();
    if (!shaderProgram) {
        
    }
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        TR_CORE_ERROR("Shader Program Linking Failed: {0}", infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    f32 vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };

    u32 indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    u32 VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(s_window)) {
        on_update();


        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized

        on_render();

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        glfwSwapBuffers(s_window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    on_shutdown();
}

}
