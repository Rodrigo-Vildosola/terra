#include "terrapch.h"
#include "terra/platform/macos/window.h"

#include "terra/events/application_event.h"
#include "terra/events/mouse_event.h"
#include "terra/events/key_event.h"
#include "terra/renderer/renderer.h"

namespace terra {

	static uint8_t s_GLFWWindowCount = 0;

	static void GLFWErrorCallback(int error, const char* description)
	{
		TR_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	MacOSWindow::MacOSWindow(const WindowProps& props)
	{
		init(props);
	}

	MacOSWindow::~MacOSWindow()
	{
		// shutdown();
	}

	WGPUSurface MacOSWindow::get_surface(WGPUInstance wgpu_instance) const {
		return glfwCreateWindowWGPUSurface(wgpu_instance, m_window);
	}

	void MacOSWindow::init(const WindowProps& props)
	{
		m_data.title = props.title;
		m_data.width = props.width;
		m_data.height = props.height;

		TR_CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);

		if (s_GLFWWindowCount == 0)
		{
			TR_CORE_INFO("Initializing GLFW");

			int success = glfwInit();
			TR_CORE_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		{
			// #if defined(TR_DEBUG)
			// if (Renderer::GetAPI() == RendererAPI::API::OpenGL)
			// 	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
			// #endif
			m_window = glfwCreateWindow((int)props.width, (int)props.height, m_data.title.c_str(), nullptr, nullptr);
			TR_CORE_ASSERT(m_window, "Failed to create GLFW window");
			++s_GLFWWindowCount;
		}

		glfwSetWindowUserPointer(m_window, &m_data);
		set_vsync(true);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.width = width;
			data.height = height;

			WindowResizeEvent event(width, height);
			data.event_cb(event);
		});

		glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.event_cb(event);
		});

		glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.event_cb(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.event_cb(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.event_cb(event);
					break;
				}
			}
		});

		glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(keycode);
			data.event_cb(event);
		});

		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.event_cb(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.event_cb(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.event_cb(event);
		});

		glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.event_cb(event);
		});
	}

	void MacOSWindow::shutdown()
	{
		glfwDestroyWindow(m_window);
		--s_GLFWWindowCount;
		if (s_GLFWWindowCount == 0)
		{
			TR_CORE_INFO("Terminating GLFW");
			glfwTerminate();
		}
	}

	void MacOSWindow::on_update()
	{
		// int win_w, win_h;
		// int fb_w, fb_h;
		// glfwGetWindowSize(m_window, &win_w, &win_h);
		// glfwGetFramebufferSize(m_window, &fb_w, &fb_h);

		// TR_CORE_INFO("Window size = {}x{}, Framebuffer size = {}x{}", win_w, win_h, fb_w, fb_h);

        glfwPollEvents();
        // m_context->swap_buffers();
	}

	std::pair<u32, u32> MacOSWindow::get_framebuffer_size() const {
		int width, height;
		glfwGetFramebufferSize(m_window, &width, &height);
		return { (u32)width, (u32)height };
	}


	void MacOSWindow::set_vsync(bool enabled)
	{
		m_data.vsync = enabled;
	}

	bool MacOSWindow::is_vsync() const
	{
		return m_data.vsync;
	}

}
