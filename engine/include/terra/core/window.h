#pragma once

#include <sstream>
#include <functional>
#include "terra/core/base.h"
#include "terra/events/event.h"

namespace terra {

	struct WindowProps
	{
		std::string title;
		uint32_t width;
		uint32_t height;

		WindowProps(const std::string& title = "Terra Engine",
			        uint32_t width = 900,
			        uint32_t height = 900)
			: title(title), width(width), height(height) {}
	};

	// Interface representing a desktop system based Window
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void on_update() = 0;

		virtual uint32_t get_width() const = 0;
		virtual uint32_t get_height() const = 0;

		// Window attributes
		virtual void set_event_cb(const EventCallbackFn& callback) = 0;
		virtual void set_vsync(bool enabled) = 0;
		virtual bool is_vsync() const = 0;

		virtual void* get_native_window() const = 0;

		virtual WGPUSurface get_surface(WGPUInstance wgpu_instance) const = 0;


		static scope<Window> create(const WindowProps& props = WindowProps());
	};

}
