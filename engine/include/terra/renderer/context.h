#pragma once

#include "terrapch.h"
#include <webgpu/webgpu.hpp>

namespace terra {

class Window; // Forward declaration to avoid circular includes

struct ContextProps {
    // Placeholder for future context settings like:
    // bool enableValidation = true;
    // std::string preferredAdapterName;
};

class WebGPUContext {
public:
    explicit WebGPUContext(const ContextProps& props = ContextProps());
    ~WebGPUContext();

    void init(Window* window_handle);
    void swap_buffers();

    static scope<WebGPUContext> create(const ContextProps& props = ContextProps());

private:
    Window* m_window_handle = nullptr;
    ContextProps m_props;

    WGPUInstance m_instance = nullptr;

    void create_swap_chain();
};

} // namespace terra
