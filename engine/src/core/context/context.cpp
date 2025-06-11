

#include "terrapch.h"
#include "terra/renderer/renderer.h"

#include "terra/core/context/context.h"
#include "terra/core/context/context_utils.h"
#include "terra/core/context/macros.h"


// #include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace terra {

WebGPUContext::WebGPUContext(const ContextProps& props)
    : m_props(props)
{}

WebGPUContext::~WebGPUContext() {
    if (m_device) wgpuDeviceRelease(m_device);
    if (m_surface) wgpuSurfaceRelease(m_surface);
}


void WebGPUContext::init(Window* window_handle) {
	TR_CORE_ASSERT(window_handle, "Window handle is null!");

	m_window_handle = window_handle;
	WGPUInstanceDescriptor desc = {};
	desc.nextInChain = nullptr;

	// Create instance
#ifdef WEBGPU_BACKEND_EMSCRIPTEN
	WGPUInstance instance = wgpuCreateInstance(nullptr);
#else //  WEBGPU_BACKEND_EMSCRIPTEN
	WGPUInstance instance = wgpuCreateInstance(&desc);
#endif //  WEBGPU_BACKEND_EMSCRIPTEN

	TR_CORE_ASSERT(instance, "WebGPU instance not yet created");

    m_surface = window_handle->get_surface(instance);

	WGPURequestAdapterOptions adapter_opts = {};
	adapter_opts.nextInChain = nullptr;
    adapter_opts.compatibleSurface = m_surface;
	WGPUAdapter adapter = request_adapter_sync(instance, &adapter_opts);

	inspect_adapter(adapter);

	wgpuInstanceRelease(instance);

	DeviceDescriptorBuilder builder;
    builder
      .label("TerraDevice")
      .default_queue_label("MainQueue")
      .on_device_lost( on_device_lost, this )
      .on_uncaptured_error( on_uncaptured_error, this );

    auto device_desc = builder.build();

	m_device = request_device_sync(adapter, &device_desc);

    inspect_device(m_device);

    wgpuAdapterRelease(adapter);

    m_queue = CommandQueue::create();
    m_queue->init(m_device);

}

void WebGPUContext::create_swap_chain() {
	
}

void WebGPUContext::swap_buffers() {
	// Present logic would go here
	// In a real app, you'd acquire the next texture and render to it
}

scope<WebGPUContext> WebGPUContext::create(const ContextProps& props) {
    return create_scope<WebGPUContext>(props);
}

}
