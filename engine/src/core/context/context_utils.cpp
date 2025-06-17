#include "terra/core/context/context_utils.h"
#include "terra/core/context/macros.h"
#include "terra/helpers/user_data.h"

namespace terra {


void wgpu_poll_events(WGPUDevice device, bool yield_to_browser) {
    #if defined(WEBGPU_BACKEND_DAWN)
        wgpuDeviceTick(device);
    #elif defined(WEBGPU_BACKEND_WGPU)
        wgpuDevicePoll(device, false, nullptr);
    #elif defined(WEBGPU_BACKEND_EMSCRIPTEN)
        if (yield_to_browser) {
            emscripten_sleep(100);
        }
    #endif
}


void sleep_for_ms(unsigned int milliseconds) {
    #ifdef __EMSCRIPTEN__
        emscripten_sleep(milliseconds);
    #else
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    #endif
}

WGPUAdapter request_adapter_sync(WGPUInstance instance, WGPURequestAdapterOptions const* options) {
	// A simple structure holding the local information shared with the
	// onAdapterrequest_ended callback.
	request_userdata<WGPUAdapter> userData;

	// Callback called by wgpuInstanceRequestAdapter when the request returns
	// This is a C++ lambda function, but could be any function defined in the
	// global scope. It must be non-capturing (the brackets [] are empty) so
	// that it behaves like a regular C function pointer, which is what
	// wgpuInstanceRequestAdapter expects (WebGPU being a C API). The workaround
	// is to convey what we want to capture through the pUserData pointer,
	// provided as the last argument of wgpuInstanceRequestAdapter and received
	// by the callback as its last argument.

	WGPURequestAdapterCallbackInfo callback_info = WGPU_REQUEST_ADAPTER_CALLBACK_INFO_INIT;
    callback_info.nextInChain = nullptr;
    callback_info.mode        = WGPUCallbackMode_AllowProcessEvents;
    callback_info.callback    = request_adapter_callback;
    callback_info.userdata1   = &userData;
    callback_info.userdata2   = nullptr;


	// Call to the WebGPU request adapter procedure
	wgpuInstanceRequestAdapter(
		instance /* equivalent of navigator.gpu */,
		options,
		callback_info
	);

	// Hand the execution to the WebGPU instance so that it can check for
	// pending async operations, in which case it invokes our callbacks.
	// NB: We test once before the loop not to wait for 200ms in case it is
	// already ready
	wgpuInstanceProcessEvents(instance);

	while (!userData.request_ended) {
		// Waiting for 200 ms to avoid asking too often to process events
		sleep_for_ms(200);

		wgpuInstanceProcessEvents(instance);
	}

	TR_CORE_ASSERT(userData.request_ended, "Request for adapter has not ended");

    return userData.result;

}

WGPUDevice request_device_sync(WGPUInstance instance, WGPUAdapter adapter, WGPUDeviceDescriptor const* descriptor) {
    request_userdata<WGPUDevice> userData;

	WGPURequestDeviceCallbackInfo callback_info = WGPU_REQUEST_DEVICE_CALLBACK_INFO_INIT;
	callback_info.nextInChain = nullptr;
	callback_info.mode        = WGPUCallbackMode_AllowProcessEvents;
	callback_info.callback    = request_device_callback;
	callback_info.userdata1   = &userData;
	callback_info.userdata2   = nullptr;

    wgpuAdapterRequestDevice(
        adapter,
        descriptor,
        callback_info
    );

	wgpuInstanceProcessEvents(instance);

	while (!userData.request_ended) {
		// Waiting for 200 ms to avoid asking too often to process events
		sleep_for_ms(200);
		wgpuInstanceProcessEvents(instance);
	}

	TR_CORE_ASSERT(userData.request_ended, "Request for device has not ended");

    return userData.result;
}


void inspect_adapter(WGPUAdapter adapter) {
#ifndef __EMSCRIPTEN__
	WGPULimits supported_limits = WGPU_LIMITS_INIT;
	supported_limits.nextInChain = nullptr;

#ifdef WEBGPU_BACKEND_DAWN
	bool success = wgpuAdapterGetLimits(adapter, &supported_limits) == WGPUStatus_Success;
#else
	bool success = wgpuAdapterGetLimits(adapter, &supported_limits);
#endif
	if (success) {
		TR_FILE_INFO("Adapter limits:");
		TR_FILE_TRACE("  maxTextureDimension1D   = {}", supported_limits.maxTextureDimension1D);
		TR_FILE_TRACE("  maxTextureDimension2D   = {}", supported_limits.maxTextureDimension2D);
		TR_FILE_TRACE("  maxTextureDimension3D   = {}", supported_limits.maxTextureDimension3D);
		TR_FILE_TRACE("  maxTextureArrayLayers   = {}", supported_limits.maxTextureArrayLayers);
	} else {
		TR_CORE_WARN("Could not query adapter limits");
	}
#endif // NOT __EMSCRIPTEN__

    // 2) Features
    {
        WGPUSupportedFeatures feats = WGPU_SUPPORTED_FEATURES_INIT;
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
        WGPUAdapterInfo info = WGPU_ADAPTER_INFO_INIT;
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

void inspect_device(WGPUDevice device) {
#ifndef __EMSCRIPTEN__
    // 1) Features
    {
        WGPUSupportedFeatures feats = WGPU_SUPPORTED_FEATURES_INIT;
        feats.features = nullptr;
        wgpuDeviceGetFeatures(device, &feats);

        TR_CORE_INFO("Device features (count={}):", feats.featureCount);
        for (uint32_t i = 0; i < feats.featureCount; ++i) {
            auto f = feats.features[i];
            TR_CORE_TRACE("  - {} (0x{:X})", feature_name_to_string(f), (u32)f);
        }

        wgpuSupportedFeaturesFreeMembers(feats);
    }

    // 2) Limits
    {
        WGPULimits limits = WGPU_LIMITS_INIT;
        limits.nextInChain = nullptr;

    #ifdef WEBGPU_BACKEND_DAWN
        bool ok = (wgpuDeviceGetLimits(device, &limits) == WGPUStatus_Success);
    #else
        bool ok = wgpuDeviceGetLimits(device, &limits);
    #endif

        if (ok) {
            TR_CORE_INFO("Device limits:");
            TR_CORE_TRACE("  maxTextureDimension1D   = {}", limits.maxTextureDimension1D);
            TR_CORE_TRACE("  maxTextureDimension2D   = {}", limits.maxTextureDimension2D);
            TR_CORE_TRACE("  maxTextureDimension3D   = {}", limits.maxTextureDimension3D);
            TR_CORE_TRACE("  maxTextureArrayLayers   = {}", limits.maxTextureArrayLayers);
            // extra device limits you might care about:
            TR_CORE_TRACE("  maxComputeWorkgroupSize = {} x {} x {}",
                limits.maxComputeWorkgroupSizeX,
                limits.maxComputeWorkgroupSizeY,
                limits.maxComputeWorkgroupSizeZ);
        } else {
            TR_CORE_WARN("Could not query device limits");
        }
    }
#endif
}

WGPUTextureFormat inspect_surface_capabilities(WGPUSurface surface, WGPUAdapter adapter) {
    TR_CORE_INFO("Inspecting surface capabilities...");

    WGPUSurfaceCapabilities capabilities = WGPU_SURFACE_CAPABILITIES_INIT;
    capabilities.nextInChain = nullptr;

    WGPUStatus status = wgpuSurfaceGetCapabilities(surface, adapter, &capabilities);
    if (status != WGPUStatus_Success) {
        TR_CORE_ERROR("Failed to get surface capabilities (status = {})", static_cast<int>(status));
        return WGPUTextureFormat_Undefined;
    }

    TR_CORE_INFO("Surface Capabilities:");
    TR_CORE_TRACE("  Texture usage bitmask: 0x{:X}", capabilities.usages);

    // 1. Formats
    TR_CORE_TRACE("  Supported formats ({}):", capabilities.formatCount);
    for (size_t i = 0; i < capabilities.formatCount; ++i) {
        TR_CORE_TRACE("    - {}", texture_format_to_string(capabilities.formats[i]));
    }

    // 2. Present Modes
    TR_CORE_TRACE("  Supported present modes ({}):", capabilities.presentModeCount);
    for (size_t i = 0; i < capabilities.presentModeCount; ++i) {
        TR_CORE_TRACE("    - {}", present_mode_to_string(capabilities.presentModes[i]));
    }

    // 3. Alpha Modes
    TR_CORE_TRACE("  Supported alpha modes ({}):", capabilities.alphaModeCount);
    for (size_t i = 0; i < capabilities.alphaModeCount; ++i) {
        TR_CORE_TRACE("    - {}", alpha_mode_to_string(capabilities.alphaModes[i]));
    }

    WGPUTextureFormat preferred_format = capabilities.formats[0];

    wgpuSurfaceCapabilitiesFreeMembers(capabilities);

    // The first one is always the preferred format!
    return preferred_format;
}


}
