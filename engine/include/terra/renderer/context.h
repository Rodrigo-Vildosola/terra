#pragma once

#include "terrapch.h"

struct GLFWwindow;

namespace terra {

	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;

		virtual void init() = 0;
		virtual void swap_buffers() = 0;

		static scope<GraphicsContext> create(void* window);

	};

    class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* window_handle);

		virtual void init() override;
		virtual void swap_buffers() override;
	private:
		GLFWwindow* m_window_handle;
	};

}
