#pragma once

#include "terrapch.h"
#include <webgpu/webgpu.hpp>
#ifdef WEBGPU_BACKEND_WGPU
    #include <webgpu/wgpu.h>
#endif // WEBGPU_BACKEND_WGPU


namespace terra {

class RenderQueueProps {

};

class RenderQueue {
public:

    explicit RenderQueue(const RenderQueueProps& props = RenderQueueProps());
    ~RenderQueue();

    void init(const WGPUDevice device);
    void submit();


    static scope<RenderQueue> create(const RenderQueueProps& props = RenderQueueProps());


private:
    WGPUQueue m_command_queue = nullptr;
    WGPUCommandEncoder m_encoder = nullptr;
    


};
    
} // namespace terra
