#pragma once

#include "terrapch.h"
#include <webgpu/webgpu.hpp>

namespace terra {

class Window;
class CommandQueue;

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

    WGPUDevice get_native_device() { return m_device; }
    CommandQueue* get_queue() { return m_queue.get(); }

    std::pair<WGPUSurfaceTexture, WGPUTextureView> get_next_surface_view();

    void configure_surface(WGPUTextureFormat preferred_format);

    WGPUTextureFormat get_preferred_format() const { return m_surface_format; }


    static scope<WebGPUContext> create(const ContextProps& props = ContextProps());

private:
    Window* m_window_handle = nullptr;
    ContextProps m_props;
    
    WGPUInstance m_instance = nullptr;
    // WGPUAdapter m_adapter = nullptr;
    WGPUDevice  m_device = nullptr;
    WGPUSurface m_surface = nullptr;
    WGPUTextureFormat m_surface_format = WGPUTextureFormat_Undefined;

    scope<CommandQueue> m_queue;


};

} // namespace terra
