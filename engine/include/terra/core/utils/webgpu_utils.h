#pragma once

#include "terrapch.h"

#include "webgpu/webgpu.h"


namespace terra {


#define FOREACH_WGPU_FEATURE_NAME(X)                            \
    X(DepthClipControl)                                         \
    X(Depth32FloatStencil8)                                     \
    X(TimestampQuery)                                           \
    X(TextureCompressionBC)                                     \
    X(TextureCompressionBCSliced3D)                             \
    X(TextureCompressionETC2)                                   \
    X(TextureCompressionASTC)                                   \
    X(TextureCompressionASTCSliced3D)                           \
    X(IndirectFirstInstance)                                    \
    X(ShaderF16)                                                \
    X(RG11B10UfloatRenderable)                                  \
    X(BGRA8UnormStorage)                                        \
    X(Float32Filterable)                                        \
    X(Float32Blendable)                                         \
    X(ClipDistances)                                            \
    X(DualSourceBlending)                                       \
    X(Subgroups)                                                \
    X(CoreFeaturesAndLimits)                                    \
    X(DawnInternalUsages)                                       \
    X(DawnMultiPlanarFormats)                                   \
    X(DawnNative)                                               \
    X(ChromiumExperimentalTimestampQueryInsidePasses)           \
    X(ImplicitDeviceSynchronization)                            \
    X(TransientAttachments)                                     \
    X(MSAARenderToSingleSampled)                                \
    X(D3D11MultithreadProtected)                                \
    X(ANGLETextureSharing)                                      \
    X(PixelLocalStorageCoherent)                                \
    X(PixelLocalStorageNonCoherent)                             \
    X(Unorm16TextureFormats)                                    \
    X(Snorm16TextureFormats)                                    \
    X(MultiPlanarFormatExtendedUsages)                          \
    X(MultiPlanarFormatP010)                                    \
    X(HostMappedPointer)                                        \
    X(MultiPlanarRenderTargets)                                 \
    X(MultiPlanarFormatNv12a)                                   \
    X(FramebufferFetch)                                         \
    X(BufferMapExtendedUsages)                                  \
    X(AdapterPropertiesMemoryHeaps)                             \
    X(AdapterPropertiesD3D)                                     \
    X(AdapterPropertiesVk)                                      \
    X(R8UnormStorage)                                           \
    X(DawnFormatCapabilities)                                   \
    X(DawnDrmFormatCapabilities)                                \
    X(Norm16TextureFormats)                                     \
    X(MultiPlanarFormatNv16)                                    \
    X(MultiPlanarFormatNv24)                                    \
    X(MultiPlanarFormatP210)                                    \
    X(MultiPlanarFormatP410)                                    \
    X(SharedTextureMemoryVkDedicatedAllocation)                 \
    X(SharedTextureMemoryAHardwareBuffer)                       \
    X(SharedTextureMemoryDmaBuf)                                \
    X(SharedTextureMemoryOpaqueFD)                              \
    X(SharedTextureMemoryZirconHandle)                          \
    X(SharedTextureMemoryDXGISharedHandle)                      \
    X(SharedTextureMemoryD3D11Texture2D)                        \
    X(SharedTextureMemoryIOSurface)                             \
    X(SharedTextureMemoryEGLImage)                              \
    X(SharedFenceVkSemaphoreOpaqueFD)                           \
    X(SharedFenceSyncFD)                                        \
    X(SharedFenceVkSemaphoreZirconHandle)                       \
    X(SharedFenceDXGISharedHandle)                              \
    X(SharedFenceMTLSharedEvent)                                \
    X(SharedBufferMemoryD3D12Resource)                          \
    X(StaticSamplers)                                           \
    X(YCbCrVulkanSamplers)                                      \
    X(ShaderModuleCompilationOptions)                           \
    X(DawnLoadResolveTexture)                                   \
    X(DawnPartialLoadResolveTexture)                            \
    X(MultiDrawIndirect)                                        \
    X(DawnTexelCopyBufferRowAlignment)                          \
    X(FlexibleTextureViews)                                     \
    X(ChromiumExperimentalSubgroupMatrix)                       \
    X(SharedFenceEGLSync)                                       \
    X(DawnDeviceAllocatorControl)                               \
    X(Force32)

#define FOREACH_WGPU_BACKEND_TYPE(X)    \
    X(Undefined)                        \
    X(Null)                             \
    X(WebGPU)                           \
    X(D3D11)                            \
    X(D3D12)                            \
    X(Metal)                            \
    X(Vulkan)                           \
    X(OpenGL)                           \
    X(OpenGLES)                         \
    X(Force32)

#define FOREACH_WGPU_ADAPTER_TYPE(X)    \
    X(DiscreteGPU)                      \
    X(IntegratedGPU)                    \
    X(CPU)                              \
    X(Unknown)                          \
    X(Force32)


static const char* feature_name_to_string(WGPUFeatureName f) {
    switch (f) {
#define MAKE_CASE(name) \
        case WGPUFeatureName_##name: return #name;
        FOREACH_WGPU_FEATURE_NAME(MAKE_CASE)
#undef MAKE_CASE

        default: {
            static char buf[32];
            snprintf(buf, sizeof(buf), "Unknown(0x%X)", (u32) f);
            return buf;
        }
    }
}

static const char* backend_type_to_string(WGPUBackendType f) {
    switch (f) {
#define MAKE_CASE(name) \
        case WGPUBackendType_##name: return #name;
        FOREACH_WGPU_BACKEND_TYPE(MAKE_CASE)
#undef MAKE_CASE

        default: {
            static char buf[32];
            snprintf(buf, sizeof(buf), "Unknown(0x%X)", (u32) f);
            return buf;
        }
    }
}

static const char* adapter_type_to_string(WGPUAdapterType f) {
    switch (f) {
#define MAKE_CASE(name) \
        case WGPUAdapterType_##name: return #name;
        FOREACH_WGPU_ADAPTER_TYPE(MAKE_CASE)
#undef MAKE_CASE

        default: {
            static char buf[32];
            snprintf(buf, sizeof(buf), "Unknown(0x%X)", (u32) f);
            return buf;
        }
    }
}

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
