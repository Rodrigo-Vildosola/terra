#pragma once

#include "terrapch.h"
#include <webgpu/webgpu.hpp>
#ifdef WEBGPU_BACKEND_WGPU
    #include <webgpu/wgpu.h>
#endif // WEBGPU_BACKEND_WGPU


namespace terra {

class CommandQueueProps {

};

class CommandQueue {
public:

    explicit CommandQueue(const CommandQueueProps& props = CommandQueueProps());
    ~CommandQueue();

    void init(const WGPUDevice device);
    void begin_frame(std::string_view label = "Frame Command Encoder");
    void end_frame();

    void add_marker(std::string_view label);
    void submit_now();

    void poll(); // Poll/tick device for async processing

    static scope<CommandQueue> create(const CommandQueueProps& props = CommandQueueProps());


private:
    WGPUDevice m_device = nullptr;
    WGPUQueue m_queue = nullptr;
    WGPUCommandEncoder m_encoder = nullptr;

    bool m_frame_active = false;
};
    
} // namespace terra
