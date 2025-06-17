#include "terra/renderer/pipeline.h"
#include "terra/helpers/error.h"
#include "terra/helpers/string.h"
#include "terra/helpers/user_data.h"

const char* shader_source = R"(
@vertex
fn vs_main(@location(0) in_vertex_position: vec2f) -> @builtin(position) vec4f {
    return vec4f(in_vertex_position, 0.0, 1.0);
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
	auto shader = Shader::create_from_wgsl(
		m_context,
		shader_source,
		"Triangle Shader Module"
	);
	WGPUShaderModule shader_module = shader.module();


	WGPURenderPipelineDescriptor pipeline_desc = WGPU_RENDER_PIPELINE_DESCRIPTOR_INIT;

	// Each sequence of 3 vertices is considered as a triangle
	pipeline_desc.primitive.topology = WGPUPrimitiveTopology_TriangleList;

	// We'll see later how to specify the order in which vertices should be
	// connected. When not specified, vertices are considered sequentially.
	pipeline_desc.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;

	// The face orientation is defined by assuming that when looking
	// from the front of the face, its corner vertices are enumerated
	// in the counter-clockwise (CCW) order.
	pipeline_desc.primitive.frontFace = WGPUFrontFace_CCW;

	// But the face orientation does not matter much because we do not
	// cull (i.e. "hide") the faces pointing away from us (which is often
	// used for optimization).
	pipeline_desc.primitive.cullMode = WGPUCullMode_None;

	// Vertex state
	WGPUVertexState vertex_state = WGPU_VERTEX_STATE_INIT;
	vertex_state.module = shader_module;
	vertex_state.entryPoint = "vs_main"_wgpu;

	WGPUVertexAttribute pos_attr = WGPU_VERTEX_ATTRIBUTE_INIT;
	pos_attr.shaderLocation = 0;
	pos_attr.format         = WGPUVertexFormat_Float32x2;
	pos_attr.offset         = 0;

	WGPUVertexBufferLayout vb_layout = WGPU_VERTEX_BUFFER_LAYOUT_INIT;
	vb_layout.arrayStride   = sizeof(float) * 2; // x,y
	vb_layout.stepMode = WGPUVertexStepMode_Vertex;
	vb_layout.attributeCount= 1;
	vb_layout.attributes    = &pos_attr;

	vertex_state.bufferCount = 1;
	vertex_state.buffers     = &vb_layout;

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
