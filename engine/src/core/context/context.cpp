

#include "terrapch.h"
#include "terra/renderer/renderer.h"

#include "terra/core/context/context.h"
#include "terra/core/context/context_utils.h"
#include "terra/core/context/macros.h"

#include "terra/helpers/string.h"


// #include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace terra {

WebGPUContext::WebGPUContext(const ContextProps& props)
    : m_props(props)
{}

WebGPUContext::~WebGPUContext() {
	wgpuSurfaceUnconfigure(m_surface);

    if (m_device) wgpuDeviceRelease(m_device);
    if (m_surface) wgpuSurfaceRelease(m_surface);
}


void WebGPUContext::init(Window* window_handle) {
	TR_CORE_ASSERT(window_handle, "Window handle is null!");

	m_window_handle = window_handle;
	WGPUInstanceDescriptor desc = {};
	desc.nextInChain = nullptr;

	// Create instance
#ifdef WEBGPU_BACKEND_EMSCRIPTEN
	WGPUInstance instance = wgpuCreateInstance(nullptr);
#else //  WEBGPU_BACKEND_EMSCRIPTEN
	WGPUInstance instance = wgpuCreateInstance(&desc);
#endif //  WEBGPU_BACKEND_EMSCRIPTEN

	TR_CORE_ASSERT(instance, "WebGPU instance not yet created");

    m_surface = window_handle->get_surface(instance);

	WGPURequestAdapterOptions adapter_opts = {};
	adapter_opts.nextInChain = nullptr;
    adapter_opts.compatibleSurface = m_surface;
	WGPUAdapter adapter = request_adapter_sync(instance, &adapter_opts);

	inspect_adapter(adapter);

	wgpuInstanceRelease(instance);

	DeviceDescriptorBuilder builder;
    builder
      .label("TerraDevice")
      .default_queue_label("MainQueue")
      .on_device_lost( on_device_lost, this )
      .on_uncaptured_error( on_uncaptured_error, this );

    auto device_desc = builder.build();

	m_device = request_device_sync(adapter, &device_desc);

    inspect_device(m_device);

	m_queue = CommandQueue::create();
    m_queue->init(m_device);

	inspect_surface_capabilities(m_surface, adapter);
	configure_surface();

    wgpuAdapterRelease(adapter);

}

void WebGPUContext::configure_surface() {
	TR_CORE_INFO("Configuring swap chain...");

    auto [fb_width, fb_height] = m_window_handle->get_framebuffer_size();

	WGPUTextureFormat format = WGPUTextureFormat_BGRA8Unorm;

	WGPUSurfaceConfiguration config = {};
    config.device       = m_device;
    config.format       = format;
    config.usage        = WGPUTextureUsage_RenderAttachment;
    config.width        = fb_width;
    config.height       = fb_height;
    config.presentMode  = WGPUPresentMode_Fifo;
    config.alphaMode    = WGPUCompositeAlphaMode_Auto;
    config.viewFormatCount = 0;
    config.viewFormats     = nullptr;

	wgpuSurfaceConfigure(m_surface, &config);

    TR_CORE_INFO("Surface configured: {}x{} @ format {}", fb_width, fb_height, (i32)format);
}

std::pair<WGPUSurfaceTexture, WGPUTextureView> WebGPUContext::get_next_surface_view() {
    WGPUSurfaceTexture surface_texture = {};
    wgpuSurfaceGetCurrentTexture(m_surface, &surface_texture);

	if (surface_texture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal &&
		surface_texture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessSuboptimal) {
		TR_CORE_ERROR("Failed to acquire surface texture! Status = {}", (int)surface_texture.status);
		return { surface_texture, nullptr };
	}
	if (surface_texture.status == WGPUSurfaceGetCurrentTextureStatus_SuccessSuboptimal) {
		TR_CORE_WARN("Acquired surface texture is suboptimal. You should reconfigure the surface.");
	}

	WGPUTextureViewDescriptor view_desc = {};
    view_desc.nextInChain = nullptr;
    view_desc.label = "SurfaceView"_wgpu;
    view_desc.format = wgpuTextureGetFormat(surface_texture.texture);
    view_desc.dimension = WGPUTextureViewDimension_2D;
    view_desc.baseMipLevel = 0;
    view_desc.mipLevelCount = 1;
    view_desc.baseArrayLayer = 0;
    view_desc.arrayLayerCount = 1;
    view_desc.aspect = WGPUTextureAspect_All;

    WGPUTextureView texture_view = wgpuTextureCreateView(surface_texture.texture, &view_desc);

#ifndef WEBGPU_BACKEND_WGPU
    wgpuTextureRelease(surface_texture.texture); // Texture view keeps ref
#endif

    return { surface_texture, texture_view };

}

void WebGPUContext::swap_buffers() {
	// Present logic would go here
	// In a real app, you'd acquire the next texture and render to it
	wgpuSurfacePresent(m_surface); // 🔥 Add this here
}

scope<WebGPUContext> WebGPUContext::create(const ContextProps& props) {
    return create_scope<WebGPUContext>(props);
}

}
