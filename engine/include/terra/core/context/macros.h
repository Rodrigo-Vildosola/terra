#pragma once

#include "terra/helpers/user_data.h"

namespace terra {

#define FOREACH_SYNC_REQUEST(cb_name, StatusType, HandleType, SuccessEnum)  \
    cb_name##_callback,                                                     \
    void cb_name##_callback(StatusType status, HandleType handle,           \
                            WGPUStringView message,                         \
                            void* userdata1, void* userdata2)

#define DEFINE_SYNC_REQUEST_CALLBACK(fn, StatusType, HandleType, SuccessEnum)           \
static void fn##_callback(                                                              \
    StatusType       status,                                                            \
    HandleType       handle,                                                            \
    WGPUStringView   message,                                                           \
    void*            userdata1,                                                         \
    void*            /*userdata2*/)                                                     \
{                                                                                       \
    using UD = request_userdata<HandleType>;                                            \
    auto* ud = reinterpret_cast<UD*>(userdata1);                                        \
    if (status == SuccessEnum) {                                                        \
        ud->result = handle;                                                            \
    } else {                                                                            \
        std::string_view msg{message.data, message.length};                             \
        TR_CORE_ERROR("{} failed: {}", #fn, msg);                                       \
    }                                                                                   \
    ud->request_ended = true;                                                           \
}

#define DEFINE_ERROR_CALLBACK(fn, ReasonType, EnumSuffix)                        \
static void fn(                                                                  \
    const WGPUDevice* device,                                                    \
    ReasonType        reason,                                                    \
    WGPUStringView    message,                                                   \
    void*             /*userdata1*/,                                             \
    void*             /*userdata2*/)                                             \
{                                                                                \
    std::string msg{message.data, message.length};                               \
    if (msg.empty()) {                                                           \
        TR_CORE_ERROR("{}: 0x{:X}", #fn, (u32)reason);                           \
    } else {                                                                     \
        TR_CORE_ERROR("{}: 0x{:X}: {}", #fn, (u32)reason, msg);                  \
    }                                                                            \
}

DEFINE_SYNC_REQUEST_CALLBACK(request_adapter, WGPURequestAdapterStatus, WGPUAdapter, WGPURequestAdapterStatus_Success)
DEFINE_SYNC_REQUEST_CALLBACK(request_device,  WGPURequestDeviceStatus,  WGPUDevice,  WGPURequestDeviceStatus_Success)

// Define them:
DEFINE_ERROR_CALLBACK(on_device_lost,       WGPUDeviceLostReason,       _Success)
DEFINE_ERROR_CALLBACK(on_uncaptured_error,  WGPUErrorType,              _Success)

#undef DEFINE_ERROR_CALLBACK

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


}
