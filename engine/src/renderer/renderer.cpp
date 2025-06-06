#include "terra/renderer/renderer.h"
#include "terra/core/logger.h"

#include <glad/glad.h>

namespace terra {

RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

u32 Renderer::s_shader_program = 0;
u32 Renderer::s_VAO = 0, Renderer::s_VBO = 0, Renderer::s_EBO = 0;

static constexpr const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
void main() {
    gl_Position = vec4(aPos, 1.0);
}
)";

static constexpr const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 0.5, 0.2, 1.0);
}
)";

void Renderer::init() {
    TR_CORE_INFO("Initializing Renderer");

    // Compile shaders
    u32 vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    u32 fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    s_shader_program = glCreateProgram();
    glAttachShader(s_shader_program, vertexShader);
    glAttachShader(s_shader_program, fragmentShader);
    glLinkProgram(s_shader_program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Geometry
    f32 vertices[] = {
         0.5f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };

    u32 indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, &s_VAO);
    glGenBuffers(1, &s_VBO);
    glGenBuffers(1, &s_EBO);

    glBindVertexArray(s_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, s_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void Renderer::begin_frame() {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::draw() {
    glUseProgram(s_shader_program);
    glBindVertexArray(s_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::end_frame() {
    // Optional: add post-processing, UI rendering, etc.
}

void Renderer::shutdown() {
    glDeleteVertexArrays(1, &s_VAO);
    glDeleteBuffers(1, &s_VBO);
    glDeleteBuffers(1, &s_EBO);
    glDeleteProgram(s_shader_program);
}

}
