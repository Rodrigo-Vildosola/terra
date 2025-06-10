

#include "terrapch.h"

#include "terra/renderer/context.h"

#include "terra/renderer/renderer.h"

// #include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

namespace terra {

WebGPUContext::WebGPUContext(const ContextProps& props)
    : m_props(props)
{}

WebGPUContext::~WebGPUContext() {
    if (m_instance) {
        wgpuInstanceRelease(m_instance);
        TR_CORE_INFO("WebGPU instance released");
    }
}


void WebGPUContext::init(Window* window_handle) {
	TR_CORE_ASSERT(window_handle, "Window handle is null!");

	m_window_handle = window_handle;
	WGPUInstanceDescriptor desc = {};
	desc.nextInChain = nullptr;

	// Create instance
#ifdef WEBGPU_BACKEND_EMSCRIPTEN
	m_instance = wgpuCreateInstance(nullptr);
#else //  WEBGPU_BACKEND_EMSCRIPTEN
	m_instance = wgpuCreateInstance(&desc);
#endif //  WEBGPU_BACKEND_EMSCRIPTEN

	TR_CORE_ASSERT(m_instance, "WebGPU instance not yet created");
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
