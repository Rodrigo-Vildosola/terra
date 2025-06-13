#include "terra/renderer/pipeline.h"
#include "terra/helpers/error.h"
#include "terra/helpers/string.h"
#include "terra/helpers/user_data.h"

const char* shader_source = R"(
@vertex
fn vs_main(@builtin(vertex_index) in_vertex_index: u32) -> @builtin(position) vec4f {
    var pos: vec4f;
    if (in_vertex_index == 0u) {
        pos = vec4f(-0.45, 0.5, 0.0, 1.0);
    } else if (in_vertex_index == 1u) {
        pos = vec4f(0.45, 0.5, 0.0, 1.0);
    } else {
        pos = vec4f(0.0, -0.5, 0.0, 1.0);
    }

    let flipY = mat4x4f(
        vec4f(1.0,  0.0, 0.0, 0.0),
        vec4f(0.0, -1.0, 0.0, 0.0),
        vec4f(0.0,  0.0, 1.0, 0.0),
        vec4f(0.0,  0.0, 0.0, 1.0),
    );

    var output: vec4f;
    output = flipY * pos;
    return output;
}

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

void Pipeline::bind(WGPURenderPassEncoder render_pass) const {
    if (!m_pipeline) {
        TR_CORE_ERROR("Tried to bind a null pipeline!");
        return;
    }

    wgpuRenderPassEncoderSetPipeline(render_pass, m_pipeline);
}


void Pipeline::create_pipeline() {
    WGPUShaderSourceWGSL wgsl_desc = WGPU_SHADER_SOURCE_WGSL_INIT;
	wgsl_desc.code = to_wgpu_string_view(shader_source);

	WGPUShaderModuleDescriptor shader_desc = WGPU_SHADER_MODULE_DESCRIPTOR_INIT;
	shader_desc.nextInChain = &wgsl_desc.chain;
	shader_desc.label = "Triangle Shader Module"_wgpu;

	request_userdata<bool> shader_compiled;

	WGPU_PUSH_ERROR_SCOPE(m_context.get_native_device());
	WGPUShaderModule shader_module = wgpuDeviceCreateShaderModule(m_context.get_native_device(), &shader_desc);
	WGPU_POP_ERROR_SCOPE_CAPTURE_BOOL(m_context.get_native_device(), &shader_compiled);


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
	pipeline_desc.primitive.topology = WGPUPrimitiveTopology_TriangleList;

	m_pipeline = wgpuDeviceCreateRenderPipeline(m_context.get_native_device(), &pipeline_desc);

    wgpuShaderModuleRelease(shader_module);

}



}
