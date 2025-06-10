

#include "terrapch.h"

#include "terra/renderer/context.h"

#include "terra/renderer/renderer.h"

// #include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

namespace terra {

scope<GraphicsContext> GraphicsContext::create(void* window)
{
	switch (Renderer::get_API())
	{
		case RendererAPI::API::None:    TR_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  TR_CORE_ASSERT(false, "RendererAPI::OpenGL is currently not supported!"); return nullptr;
		case RendererAPI::API::Vulkan:  TR_CORE_ASSERT(false, "RendererAPI::Vulkan is currently not supported!"); return nullptr;
		case RendererAPI::API::WebGPU:  return create_scope<WebGPUContext>(static_cast<GLFWwindow*>(window));
	}

	TR_CORE_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}

WebGPUContext::WebGPUContext(GLFWwindow* window_handle)
	: m_window_handle(window_handle) 
{
	TR_CORE_ASSERT(window_handle, "Window handle is null!");
}

void WebGPUContext::init()
{
	glfwGetFramebufferSize(m_window_handle, &m_width, &m_height);

	WGPUInstanceDescriptor desc = {};
	desc.nextInChain = nullptr;

	// Create instance
#ifdef WEBGPU_BACKEND_EMSCRIPTEN
	WGPUInstance m_instance = wgpuCreateInstance(nullptr);
#else //  WEBGPU_BACKEND_EMSCRIPTEN
	WGPUInstance m_instance = wgpuCreateInstance(&desc);
#endif //  WEBGPU_BACKEND_EMSCRIPTEN


	TR_CORE_INFO("WebGPU initialized with format: BGRA8Unorm");
}

void WebGPUContext::create_swap_chain()
{
	
}

void WebGPUContext::swap_buffers()
{
	// Present logic would go here
	// In a real app, you'd acquire the next texture and render to it
}

}
