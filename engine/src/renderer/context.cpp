

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
        TR_CORE_DEBUG("WebGPU instance released");
    }
	if (m_adapter) {
		wgpuAdapterRelease(m_adapter);
        TR_CORE_DEBUG("WebGPU adapter released");
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

	WGPURequestAdapterOptions adapter_opts = {};
	adapter_opts.nextInChain = nullptr;
	m_adapter = requestAdapterSync(&adapter_opts);

	wgpu::Adapter opt = (wgpu::Adapter) m_adapter;
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

WGPUAdapter WebGPUContext::requestAdapterSync(WGPURequestAdapterOptions const* options) {
	// A simple structure holding the local information shared with the
	// onAdapterRequestEnded callback.
	struct UserData {
		WGPUAdapter adapter = nullptr;
		bool requestEnded = false;
	};
	UserData userData;

	// Callback called by wgpuInstanceRequestAdapter when the request returns
	// This is a C++ lambda function, but could be any function defined in the
	// global scope. It must be non-capturing (the brackets [] are empty) so
	// that it behaves like a regular C function pointer, which is what
	// wgpuInstanceRequestAdapter expects (WebGPU being a C API). The workaround
	// is to convey what we want to capture through the pUserData pointer,
	// provided as the last argument of wgpuInstanceRequestAdapter and received
	// by the callback as its last argument.
    auto onAdapterRequestEnded = +[](
        WGPURequestAdapterStatus status,
        WGPUAdapter             adapter,
        WGPUStringView          message,
        void*                   userdata1,
        void*                   userdata2)
    {
        auto* p = reinterpret_cast<UserData*>(userdata1);
        if (status == WGPURequestAdapterStatus_Success) {
            p->adapter = adapter;
        } else {
			// Convert the WGPUStringView into a std::string
			std::string_view msg{ message.data, message.length };
			// Log the error with your macro
			TR_CORE_ERROR("wgpuInstanceRequestAdapter failed: {}", msg);
        }
        p->requestEnded = true;
    };

	WGPURequestAdapterCallbackInfo callbackInfo = {};
    callbackInfo.nextInChain = nullptr;
    callbackInfo.mode        = WGPUCallbackMode_AllowSpontaneous;
    callbackInfo.callback    = onAdapterRequestEnded;
    callbackInfo.userdata1   = &userData;
    callbackInfo.userdata2   = nullptr;


	// Call to the WebGPU request adapter procedure
	wgpuInstanceRequestAdapter(
		m_instance /* equivalent of navigator.gpu */,
		options,
		callbackInfo
	);

#ifdef __EMSCRIPTEN__
		while (!userData.requestEnded) {
			emscripten_sleep(100);
		}
#endif // __EMSCRIPTEN__

	TR_CORE_ASSERT(userData.requestEnded, "Request for adapter has not ended");

    return userData.adapter;

}

}
