#include "terra/renderer/pipeline.h"
#include "terra/helpers/string.h"

const char* shader_source = R"(
@vertex
fn vs_main(@builtin(vertex_index) in_vertex_index: u32) -> @builtin(position) vec4f {
	if (in_vertex_index == 0u) {
		return vec4f(-0.45, 0.5, 0.0, 1.0);
	} else if (in_vertex_index == 1u) {
		return vec4f(0.45, 0.5, 0.0, 1.0);
	} else {
		return vec4f(0.0, -0.5, 0.0, 1.0);
	}
}
// Add this in the same shaderSource literal than the vertex entry point
@fragment
fn fs_main() -> @location(0) vec4f {
	return vec4f(0.0, 0.4, 0.7, 1.0);
}
)";


namespace terra {

Pipeline::Pipeline(WebGPUContext& context, const PipelineSpecification& spec)
    : m_context(context), m_spec(spec) {
    create_pipeline();
}

Pipeline::~Pipeline() {
	if (m_pipeline) wgpuRenderPipelineRelease(m_pipeline);
	if (m_layout) wgpuPipelineLayoutRelease(m_layout);
	if (m_bind_group_layout) wgpuBindGroupLayoutRelease(m_bind_group_layout);
}

void Pipeline::create_pipeline() {
    WGPUShaderSourceWGSL wgsl_desc = WGPU_SHADER_SOURCE_WGSL_INIT;
	wgsl_desc.code = to_wgpu_string_view(shader_source);

	WGPUShaderModuleDescriptor shader_desc = WGPU_SHADER_MODULE_DESCRIPTOR_INIT;
	shader_desc.nextInChain = &wgsl_desc.chain;
	shader_desc.label = "Triangle Shader Module"_wgpu;

	WGPUShaderModule shader_module = wgpuDeviceCreateShaderModule(m_context.get_native_device(), &shader_desc);

	WGPURenderPipelineDescriptor pipeline_desc = WGPU_RENDER_PIPELINE_DESCRIPTOR_INIT;

	// Vertex state
	WGPUVertexState vertex_state = WGPU_VERTEX_STATE_INIT;
	vertex_state.module = shader_module;
	vertex_state.entryPoint = "vs_main"_wgpu;

	// Fragment state
	WGPUFragmentState fragment_state = WGPU_FRAGMENT_STATE_INIT;
	fragment_state.module = shader_module;
	fragment_state.entryPoint = "fs_main"_wgpu;

	WGPUColorTargetState color_target_state = WGPU_COLOR_TARGET_STATE_INIT;
	color_target_state.format = m_context.get_preferred_format();

	WGPUBlendState blend_state = WGPU_BLEND_STATE_INIT;
	color_target_state.blend = &blend_state;

	fragment_state.targetCount = 1;
	fragment_state.targets = &color_target_state;

	pipeline_desc.vertex = vertex_state;
	pipeline_desc.fragment = &fragment_state;

	m_pipeline = wgpuDeviceCreateRenderPipeline(m_context.get_native_device(), &pipeline_desc);

    wgpuShaderModuleRelease(shader_module);

}



}
