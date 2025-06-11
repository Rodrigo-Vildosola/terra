#pragma once

#include "terrapch.h"

#include "webgpu/webgpu.h"


namespace terra {

/**
 * @brief Utility function to get a WebGPU adapter, so that
 *     WGPUAdapter adapter = requestAdapterSync(options);
 * is roughly equivalent to
 *     const adapter = await navigator.gpu.requestAdapter(options);
 */
WGPUAdapter request_adapter_sync(WGPUInstance instance, WGPURequestAdapterOptions const* options);

/**
 * @brief Utility function to get a WebGPU device, so that
 *     WGPUDevice device = requestDeviceSync(adapter, options);
 * is roughly equivalent to
 *     const device = await adapter.requestDevice(descriptor);
 * It is very similar to requestAdapter
 */
WGPUDevice request_device_sync(WGPUAdapter adapter, WGPUDeviceDescriptor const* descriptor);


void inspect_adapter(WGPUAdapter adapter);
void inspect_device(WGPUDevice device);
void inspect_surface_capabilities(WGPUSurface surface, WGPUAdapter adapter);
/**
 * Helper to zero-initialize and populate a WGPUDeviceDescriptor
 * with sensible defaults, plus optional callbacks.
 */
struct DeviceDescriptorBuilder {
    WGPUDeviceDescriptor desc = {};

    DeviceDescriptorBuilder() {
        desc.nextInChain                = nullptr;
        desc.label.data                 = nullptr;
        desc.label.length               = 0;
        desc.requiredFeatureCount       = 0;
        desc.requiredFeatures           = nullptr;
        desc.requiredLimits             = nullptr;
        // defaultQueue must at least zero its own chained struct:
        desc.defaultQueue.nextInChain   = nullptr;
        desc.defaultQueue.label.data    = nullptr;
        desc.defaultQueue.label.length  = 0;
        // callbacks default to “no callback”
        desc.deviceLostCallbackInfo     = {nullptr, WGPUCallbackMode_AllowSpontaneous, nullptr, nullptr, nullptr};
        desc.uncapturedErrorCallbackInfo= {nullptr, nullptr, nullptr, nullptr};
    }

    DeviceDescriptorBuilder& label(std::string_view txt) {
        desc.label.data   = txt.data();
        desc.label.length = txt.size();
        return *this;
    }

    DeviceDescriptorBuilder& default_queue_label(std::string_view txt) {
        desc.defaultQueue.label.data   = txt.data();
        desc.defaultQueue.label.length = txt.size();
        return *this;
    }

    DeviceDescriptorBuilder& require_features(std::vector<WGPUFeatureName> const& feats) {
        desc.requiredFeatureCount = feats.size();
        desc.requiredFeatures     = feats.data();
        return *this;
    }

    // Install your “device lost” callback:
    DeviceDescriptorBuilder& on_device_lost(
        WGPUDeviceLostCallback cb, void* userdata = nullptr)
    {
        desc.deviceLostCallbackInfo.callback  = cb;
        desc.deviceLostCallbackInfo.userdata1 = userdata;
        return *this;
    }

    // Install your “uncaptured error” callback:
    DeviceDescriptorBuilder& on_uncaptured_error(
        WGPUUncapturedErrorCallback cb, void* userdata = nullptr)
    {
        desc.uncapturedErrorCallbackInfo.callback  = cb;
        desc.uncapturedErrorCallbackInfo.userdata1 = userdata;
        return *this;
    }

    // Finally, build the descriptor for passing in:
    WGPUDeviceDescriptor build() {
        return desc;
    }
};



}
