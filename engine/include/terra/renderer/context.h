#pragma once

#include "terrapch.h"
#include <webgpu/webgpu.hpp>


struct GLFWwindow;

namespace terra {

class GraphicsContext
{
public:
	virtual ~GraphicsContext() = default;

	virtual void init() = 0;
	virtual void swap_buffers() = 0;

	static scope<GraphicsContext> create(void* window);

};

class WebGPUContext : public GraphicsContext 
{
public:
	WebGPUContext(GLFWwindow* window_handle);
	~WebGPUContext();

	virtual void init() override;
	virtual void swap_buffers() override;

private:
	GLFWwindow* m_window_handle;

	WGPUInstance m_instance;
	WGPUSurface m_surface;
	WGPUAdapter m_adapter;
	WGPUDevice m_device;
	WGPUSwapChain m_swap_chain;
	WGPUQueue m_queue;
	WGPUTextureFormat m_swap_chain_format;

	int m_width;
	int m_height;

	void create_swap_chain();
};

// class OpenGLContext : public GraphicsContext
// {
// public:
// 	OpenGLContext(GLFWwindow* window_handle);

// 	virtual void init() override;
// 	virtual void swap_buffers() override;
// private:
// 	GLFWwindow* m_window_handle;
// };

}
