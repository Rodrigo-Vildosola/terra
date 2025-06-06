

#include "terrapch.h"

#include "terra/renderer/context.h"

#include "terra/renderer/renderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace terra {

	scope<GraphicsContext> GraphicsContext::create(void* window)
	{
		switch (Renderer::get_API())
		{
			case RendererAPI::API::None:    TR_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return create_scope<OpenGLContext>(static_cast<GLFWwindow*>(window));
			case RendererAPI::API::Vulkan:  TR_CORE_ASSERT(false, "RendererAPI::Vulkan is currently not supported!"); return nullptr;
		}

		TR_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}


	OpenGLContext::OpenGLContext(GLFWwindow* window_handle)
		: m_window_handle(window_handle)
	{
		TR_CORE_ASSERT(window_handle, "Window handle is null!");
	}

    void OpenGLContext::init()
    {
        glfwMakeContextCurrent(m_window_handle);
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        TR_CORE_ASSERT(status, "Failed to initialize Glad!");

        TR_CORE_INFO("OpenGL Info:");
        TR_CORE_INFO("  Vendor: {0}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
        TR_CORE_INFO("  Renderer: {0}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
        TR_CORE_INFO("  Version: {0}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    }

	void OpenGLContext::swap_buffers()
	{
		glfwSwapBuffers(m_window_handle);
	}

}
