#include "terra/core/context/context_utils.h"
#include "terra/core/context/macros.h"
#include "terra/helpers/user_data.h"

namespace terra {

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

	WGPURequestAdapterCallbackInfo callbackInfo = {};
    callbackInfo.nextInChain = nullptr;
    callbackInfo.mode        = WGPUCallbackMode_AllowSpontaneous;
    callbackInfo.callback    = request_adapter_callback;
    callbackInfo.userdata1   = &userData;
    callbackInfo.userdata2   = nullptr;


	// Call to the WebGPU request adapter procedure
	wgpuInstanceRequestAdapter(
		instance /* equivalent of navigator.gpu */,
		options,
		callbackInfo
	);

#ifdef __EMSCRIPTEN__
		while (!userData.request_ended) {
			emscripten_sleep(100);
		}
#endif // __EMSCRIPTEN__

	TR_CORE_ASSERT(userData.request_ended, "Request for adapter has not ended");

    return userData.result;

}

WGPUDevice request_device_sync(WGPUAdapter adapter, WGPUDeviceDescriptor const* descriptor) {
    request_userdata<WGPUDevice> userData;

	WGPURequestDeviceCallbackInfo callbackInfo = {};
	callbackInfo.nextInChain = nullptr;
	callbackInfo.mode        = WGPUCallbackMode_AllowSpontaneous;
	callbackInfo.callback    = request_device_callback;
	callbackInfo.userdata1   = &userData;
	callbackInfo.userdata2   = nullptr;

    wgpuAdapterRequestDevice(
        adapter,
        descriptor,
        callbackInfo
    );

#ifdef __EMSCRIPTEN__
    while (!userData.request_ended) {
        emscripten_sleep(100);
    }
#endif // __EMSCRIPTEN__

	TR_CORE_ASSERT(userData.request_ended, "Request for device has not ended");

    return userData.result;
}


void inspect_adapter(WGPUAdapter adapter) {
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

void inspect_device(WGPUDevice device) {
#ifndef __EMSCRIPTEN__
    // 1) Features
    {
        WGPUSupportedFeatures feats = {};
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
        WGPULimits limits = {};
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

}
