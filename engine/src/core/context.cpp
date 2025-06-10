

#include "terrapch.h"
#include "terra/core/context.h"

#include "terra/renderer/renderer.h"
#include "terra/core/utils/webgpu_utils.h"

// #include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

namespace terra {

WebGPUContext::WebGPUContext(const ContextProps& props)
    : m_props(props)
{}

WebGPUContext::~WebGPUContext() {
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
	m_adapter = request_adapter_sync(&adapter_opts);

	inspect_adapter(m_adapter);

	wgpuInstanceRelease(m_instance);

	auto on_device_lost = +[](
		const WGPUDevice*    /*device*/,
		WGPUDeviceLostReason reason,
		WGPUStringView       message,
		void*                /*userdata1*/,
		void*                /*userdata2*/)
	{
		// Turn the message view into a std::string
		std::string msg{ message.data, message.length };

		if (msg.empty()) {
			TR_CORE_ERROR("WebGPU device lost! Reason = 0x{:X}", (u32)reason);
		} else {
			TR_CORE_ERROR(
				"WebGPU device lost! Reason = 0x{:X}: {}", 
				(u32)reason, 
				msg
			);
		}
	};

	auto on_uncaptured_error = +[](
		const WGPUDevice* /*device*/,
		WGPUErrorType     errorType,
		WGPUStringView    message,
		void*             /*userdata1*/,
		void*             /*userdata2*/)
	{
		std::string msg{ message.data, message.length };

		if (msg.empty()) {
			TR_CORE_ERROR("WebGPU uncaptured error: type = 0x{:X}", (u32)errorType);
		} else {
			TR_CORE_ERROR(
				"WebGPU uncaptured error: type = 0x{:X}: {}", 
				(u32)errorType, 
				msg
			);
		}
	};

	DeviceDescriptorBuilder builder;
    builder
      .label("MyAppDevice")
      .default_queue_label("MainQueue")
      .on_device_lost( on_device_lost, this )
      .on_uncaptured_error( on_uncaptured_error, this );

    auto device_desc = builder.build();

	m_device = request_device_sync(&device_desc);
}

void WebGPUContext::create_swap_chain() {
	
}

void WebGPUContext::swap_buffers() {
	// Present logic would go here
	// In a real app, you'd acquire the next texture and render to it
}

WGPUAdapter WebGPUContext::request_adapter_sync(WGPURequestAdapterOptions const* options) {
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


WGPUDevice WebGPUContext::request_device_sync(WGPUDeviceDescriptor const* descriptor) {
	struct UserData {
        WGPUDevice device = nullptr;
        bool requestEnded = false;
    };
    UserData userData;

	auto onDeviceRequestEnded = +[](
        WGPURequestDeviceStatus status,
        WGPUDevice              device,
        WGPUStringView          message,
        void*                   userdata1,
        void*                   userdata2) 
	{
        auto* p = reinterpret_cast<UserData*>(userdata1);
        if (status == WGPURequestDeviceStatus_Success) {
            p->device = device;
        } else {
			// Convert the WGPUStringView into a std::string
			std::string_view msg{ message.data, message.length };
			// Log the error with your macro
			TR_CORE_ERROR("wgpuAdapterRequestDevice failed: {}", msg);
        }
        p->requestEnded = true;
    };

	WGPURequestDeviceCallbackInfo callbackInfo = {};
	callbackInfo.nextInChain = nullptr;
	callbackInfo.mode        = WGPUCallbackMode_AllowSpontaneous;
	callbackInfo.callback    = onDeviceRequestEnded;
	callbackInfo.userdata1   = &userData;
	callbackInfo.userdata2   = nullptr;

    wgpuAdapterRequestDevice(
        m_adapter,
        descriptor,
        callbackInfo
    );

#ifdef __EMSCRIPTEN__
    while (!userData.requestEnded) {
        emscripten_sleep(100);
    }
#endif // __EMSCRIPTEN__

	TR_CORE_ASSERT(userData.requestEnded, "Request for device has not ended");

    return userData.device;
}

scope<WebGPUContext> WebGPUContext::create(const ContextProps& props) {
    return create_scope<WebGPUContext>(props);
}

// ———————————————  
// Adapter callback
void WebGPUContext::OnAdapterRequestEnded(
    WGPURequestAdapterStatus status,
    WGPUAdapter             adapter,
    WGPUStringView          message,
    void*                   userdata1,
    void*                   /*userdata2*/)
{
	struct AdapterUserData {
		WGPUAdapter adapter = nullptr;
		bool requestEnded = false;
	};
    auto* p = reinterpret_cast<AdapterUserData*>(userdata1);
    if (status == WGPURequestAdapterStatus_Success) {
        p->adapter = adapter;
    } else {
        std::string msg{message.data, message.length};
        TR_CORE_ERROR("wgpuInstanceRequestAdapter failed: {}", msg);
    }
    p->requestEnded = true;
}

// ———————————————  
// Device callback
void WebGPUContext::OnDeviceRequestEnded(
    WGPURequestDeviceStatus status,
    WGPUDevice              device,
    WGPUStringView          message,
    void*                   userdata1,
    void*                   /*userdata2*/)
{
	struct DeviceUserData {
        WGPUDevice device = nullptr;
        bool requestEnded = false;
    };
    auto* p = reinterpret_cast<DeviceUserData*>(userdata1);
    if (status == WGPURequestDeviceStatus_Success) {
        p->device = device;
    } else {
        std::string msg{message.data, message.length};
        TR_CORE_ERROR("wgpuAdapterRequestDevice failed: {}", msg);
    }
    p->requestEnded = true;
}

// ———————————————  
// Device-lost callback
void WebGPUContext::OnDeviceLost(
    const WGPUDevice*    /*device*/,
    WGPUDeviceLostReason reason,
    WGPUStringView       message,
    void*                /*userdata1*/,
    void*                /*userdata2*/)
{
    std::string msg{message.data, message.length};
    if (msg.empty()) {
        TR_CORE_ERROR("WebGPU device lost! Reason = 0x{:X}", (u32)reason);
    } else {
        TR_CORE_ERROR("WebGPU device lost! Reason = 0x{:X}: {}", (u32)reason, msg);
    }
}

// ———————————————  
// Uncaptured-error callback
void WebGPUContext::OnUncapturedError(
    const WGPUDevice* /*device*/,
    WGPUErrorType     type,
    WGPUStringView    message,
    void*             /*userdata1*/,
    void*             /*userdata2*/)
{
    std::string msg{message.data, message.length};
    if (msg.empty()) {
        TR_CORE_ERROR("WebGPU uncaptured error: type=0x{:X}", (u32)type);
    } else {
        TR_CORE_ERROR("WebGPU uncaptured error: type=0x{:X}: {}", (u32)type, msg);
    }
}

void WebGPUContext::inspect_adapter(WGPUAdapter adapter) {
#ifndef __EMSCRIPTEN__
	WGPULimits supportedLimits = {};
	supportedLimits.nextInChain = nullptr;

#ifdef WEBGPU_BACKEND_DAWN
	bool success = wgpuAdapterGetLimits(adapter, &supportedLimits) == WGPUStatus_Success;
#else
	bool success = wgpuAdapterGetLimits(adapter, &supportedLimits);
#endif
	if (success) {
		TR_FILE_INFO("Adapter limits:");
		TR_FILE_TRACE("  maxTextureDimension1D   = {}", supportedLimits.maxTextureDimension1D);
		TR_FILE_TRACE("  maxTextureDimension2D   = {}", supportedLimits.maxTextureDimension2D);
		TR_FILE_TRACE("  maxTextureDimension3D   = {}", supportedLimits.maxTextureDimension3D);
		TR_FILE_TRACE("  maxTextureArrayLayers   = {}", supportedLimits.maxTextureArrayLayers);
	} else {
		TR_CORE_WARN("Could not query adapter limits");
	}
#endif // NOT __EMSCRIPTEN__

    // 2) Features
    {
        WGPUSupportedFeatures feats = {};
        feats.features = nullptr;
        wgpuAdapterGetFeatures(adapter, &feats);

        TR_CORE_INFO("Adapter features (count={}):", feats.featureCount);
        for (uint32_t i = 0; i < feats.featureCount; ++i) {
            auto f = feats.features[i];
            TR_FILE_TRACE("  - {} (0x{:X})", feature_name_to_string(f), (u32)f);
        }

        wgpuSupportedFeaturesFreeMembers(feats);
    }

	// 3) Properties
    {
        WGPUAdapterInfo info = {};
        info.nextInChain = nullptr;
        wgpuAdapterGetInfo(adapter, &info);

        TR_CORE_INFO("Adapter properties:");
        TR_CORE_TRACE("  vendorID           = {}", info.vendorID);
        if (info.vendor.data)
            TR_CORE_TRACE("  vendor             = {}", 
                std::string(info.vendor.data, info.vendor.length));
        if (info.architecture.data)
            TR_CORE_TRACE("  architecture       = {}", 
                std::string(info.architecture.data, info.architecture.length));
        TR_FILE_TRACE("  deviceID           = {}", info.deviceID);
        if (info.device.data)
            TR_CORE_TRACE("  device             = {}", 
                std::string(info.device.data, info.device.length));
        if (info.description.data)
            TR_CORE_TRACE("  description        = {}", 
                std::string(info.description.data, info.description.length));
        TR_FILE_TRACE("  subgroup sizes     = {} .. {}", 
            info.subgroupMinSize, info.subgroupMaxSize);
        TR_CORE_TRACE("  adapterType        = {} (0x{:X})", adapter_type_to_string(info.adapterType), (u32)info.adapterType);
        TR_CORE_TRACE("  backendType        = {} (0x{:X})", backend_type_to_string(info.backendType), (u32)info.backendType);
    }

}

}
