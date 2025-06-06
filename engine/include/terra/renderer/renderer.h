#pragma once

#include "terra/core/base.h"

namespace terra {

class RendererAPI {
public:
    enum class API {
        None = 0, OpenGL = 1, Vulkan = 2
    };

    inline static API get_API() { return s_API; }


private:
    static API s_API;
};

class Renderer {
public:
    static void init();
    static void shutdown();
    static void begin_frame();
    static void end_frame();
    static void draw();

    inline static RendererAPI::API get_API() { return RendererAPI::get_API(); }

private:
    static unsigned int s_shader_program;
    static unsigned int s_VAO, s_VBO, s_EBO;
};

}
