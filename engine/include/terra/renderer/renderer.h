#pragma once

#include "terra/core/base.h"
#include "terra/core/context.h"

namespace terra {

class RendererAPI {
public:
    enum class API {
        None = 0, OpenGL = 1, Vulkan = 2, WebGPU = 3
    };

    inline static API get_API() { return s_API; }


private:
    static API s_API;
};

class Renderer {
public:
    static void init(void* native_window);
    static RendererAPI::API get_API() { return RendererAPI::get_API(); }

private:
};

}
