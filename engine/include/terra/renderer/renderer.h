#pragma once

#include "terra/core/base.h"

namespace terra {

class Renderer {
public:
    static void init();
    static void shutdown();
    static void begin_frame();
    static void end_frame();
    static void draw();

private:
    static unsigned int s_shader_program;
    static unsigned int s_VAO, s_VBO, s_EBO;
};

}
