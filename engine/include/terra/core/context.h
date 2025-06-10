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

    static void inspect_adapter(WGPUAdapter adapter);

    static scope<WebGPUContext> create(const ContextProps& props = ContextProps());

private:
    Window* m_window_handle = nullptr;
    ContextProps m_props;
    
    WGPUInstance m_instance = nullptr;
    WGPUAdapter m_adapter = nullptr;
    WGPUDevice  m_device = nullptr;


    void create_swap_chain();

    /**
     * @brief Utility function to get a WebGPU adapter, so that
     *     WGPUAdapter adapter = requestAdapterSync(options);
     * is roughly equivalent to
     *     const adapter = await navigator.gpu.requestAdapter(options);
     */
    WGPUAdapter request_adapter_sync(WGPURequestAdapterOptions const* options);

    /**
     * @brief Utility function to get a WebGPU device, so that
     *     WGPUDevice device = requestDeviceSync(adapter, options);
     * is roughly equivalent to
     *     const device = await adapter.requestDevice(descriptor);
     * It is very similar to requestAdapter
     */
    WGPUDevice request_device_sync(WGPUDeviceDescriptor const* descriptor);

    // **Callbacks** moved out here:
    static void OnAdapterRequestEnded(
        WGPURequestAdapterStatus status,
        WGPUAdapter             adapter,
        WGPUStringView          message,
        void*                   userdata1,
        void*                   userdata2);

    static void OnDeviceRequestEnded(
        WGPURequestDeviceStatus status,
        WGPUDevice              device,
        WGPUStringView          message,
        void*                   userdata1,
        void*                   userdata2);

    static void OnDeviceLost(
        const WGPUDevice*     device,
        WGPUDeviceLostReason  reason,
        WGPUStringView        message,
        void*                 userdata1,
        void*                 userdata2);

    static void OnUncapturedError(
        const WGPUDevice*   device,
        WGPUErrorType       type,
        WGPUStringView      message,
        void*               userdata1,
        void*               userdata2);


};

} // namespace terra
