

#include "terrapch.h"
#include "terra/renderer/renderer.h"

#include "terra/core/context/context.h"
#include "terra/core/context/context_utils.h"
#include "terra/core/context/macros.h"

#include "terra/helpers/string.h"

namespace terra {

WebGPUContext::WebGPUContext(const ContextProps& props)
    : m_props(props)
{}

WebGPUContext::~WebGPUContext() {
	wgpuSurfaceUnconfigure(m_surface);

    if (m_device) wgpuDeviceRelease(m_device);
    if (m_surface) wgpuSurfaceRelease(m_surface);
	if (m_instance)	wgpuInstanceRelease(m_instance);

}


void WebGPUContext::init(Window* window_handle) {
	TR_CORE_ASSERT(window_handle, "Window handle is null!");

	m_window_handle = window_handle;
	WGPUInstanceDescriptor desc = WGPU_INSTANCE_DESCRIPTOR_INIT;
	desc.nextInChain = nullptr;

	// Create instance
#ifdef WEBGPU_BACKEND_EMSCRIPTEN
	m_instance = wgpuCreateInstance(nullptr);
#else //  WEBGPU_BACKEND_EMSCRIPTEN
	m_instance = wgpuCreateInstance(&desc);
#endif //  WEBGPU_BACKEND_EMSCRIPTEN

	TR_CORE_ASSERT(m_instance, "WebGPU instance not yet created");

    m_surface = window_handle->get_surface(m_instance);

	WGPURequestAdapterOptions adapter_opts = WGPU_REQUEST_ADAPTER_OPTIONS_INIT;
	adapter_opts.nextInChain = nullptr;
    adapter_opts.compatibleSurface = m_surface;
	WGPUAdapter adapter = request_adapter_sync(m_instance, &adapter_opts);

	inspect_adapter(adapter);

	DeviceDescriptorBuilder builder;
    builder
      .label("TerraDevice")
      .default_queue_label("MainQueue")
      .on_device_lost( on_device_lost, this )
      .on_uncaptured_error( on_uncaptured_error, this );

    auto device_desc = builder.build();

	m_device = request_device_sync(m_instance, adapter, &device_desc);

    inspect_device(m_device);

	m_queue = CommandQueue::create();
    m_queue->init(m_device);

	m_surface_format = inspect_surface_capabilities(m_surface, adapter);
	configure_surface(m_surface_format);

    wgpuAdapterRelease(adapter);

}

void WebGPUContext::configure_surface(WGPUTextureFormat preferred_format) {
	TR_CORE_INFO("Configuring swap chain...");

    auto [fb_width, fb_height] = m_window_handle->get_framebuffer_size();

	WGPUSurfaceConfiguration config = WGPU_SURFACE_CONFIGURATION_INIT;
    config.device       = m_device;
    config.format       = preferred_format;
    config.usage        = WGPUTextureUsage_RenderAttachment;
    config.width        = fb_width;
    config.height       = fb_height;
    config.presentMode  = WGPUPresentMode_Fifo;
    config.alphaMode    = WGPUCompositeAlphaMode_Auto;
    config.viewFormatCount = 0;
    config.viewFormats     = nullptr;

	wgpuSurfaceConfigure(m_surface, &config);

    TR_CORE_INFO("Surface configured: {}x{} @ format {}", fb_width, fb_height, (i32)preferred_format);
}

std::pair<WGPUSurfaceTexture, WGPUTextureView> WebGPUContext::get_next_surface_view() {
    WGPUSurfaceTexture surface_texture = WGPU_SURFACE_TEXTURE_INIT;
    wgpuSurfaceGetCurrentTexture(m_surface, &surface_texture);

	if (surface_texture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal &&
		surface_texture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessSuboptimal) {
		TR_CORE_ERROR("Failed to acquire surface texture! Status = {}", (int)surface_texture.status);
		return { surface_texture, nullptr };
	}
	if (surface_texture.status == WGPUSurfaceGetCurrentTextureStatus_SuccessSuboptimal) {
		TR_CORE_WARN("Acquired surface texture is suboptimal. You should reconfigure the surface.");
	}

	WGPUTextureViewDescriptor view_desc = WGPU_TEXTURE_VIEW_DESCRIPTOR_INIT;
    view_desc.nextInChain = nullptr;
    view_desc.label = "Surface texture view"_wgpu;
    view_desc.dimension = WGPUTextureViewDimension_2D;
    view_desc.format = wgpuTextureGetFormat(surface_texture.texture);
    view_desc.baseMipLevel = 0;
    view_desc.mipLevelCount = 1;
    view_desc.baseArrayLayer = 0;
    view_desc.arrayLayerCount = 1;
    view_desc.aspect = WGPUTextureAspect_All;

    WGPUTextureView texture_view = wgpuTextureCreateView(surface_texture.texture, &view_desc);

    // We no longer need the texture, only its view,
    // so we release it at the end of GetNextSurfaceViewData
    wgpuTextureRelease(surface_texture.texture);

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
