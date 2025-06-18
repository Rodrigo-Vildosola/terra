#include "terra/renderer/pipeline.h"
#include "terra/helpers/error.h"
#include "terra/helpers/string.h"
#include "terra/helpers/user_data.h"


namespace terra {

Pipeline::Pipeline(WebGPUContext& context, PipelineSpecification&& spec)
    : m_context(context), m_spec(std::move(spec)) {
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
	WGPUPipelineLayoutDescriptor layout_desc = WGPU_PIPELINE_LAYOUT_DESCRIPTOR_INIT;
    layout_desc.bindGroupLayoutCount = 0;
    layout_desc.bindGroupLayouts     = nullptr;
    m_layout = wgpuDeviceCreatePipelineLayout(
      m_context.get_native_device(),
      &layout_desc
    );

	std::vector<std::vector<WGPUVertexAttribute>>  all_attribs;
	std::vector<WGPUVertexBufferLayout>            all_layouts;
	all_attribs.reserve(m_spec.vertex_buffers.size());
	all_layouts.reserve(m_spec.vertex_buffers.size());

	for (auto const& vb : m_spec.vertex_buffers) {
		// collect attributes
		all_attribs.emplace_back();
		auto& attribs = all_attribs.back();
		for (auto const& a : vb.attributes) {
			WGPUVertexAttribute wa = WGPU_VERTEX_ATTRIBUTE_INIT;
			wa.shaderLocation = a.shader_location;
			wa.format         = a.format;
			wa.offset         = a.offset;
			attribs.push_back(wa);
		}

		// build the buffer layout
		WGPUVertexBufferLayout layout = WGPU_VERTEX_BUFFER_LAYOUT_INIT;
		layout.arrayStride    = vb.stride;
		layout.stepMode       = vb.step_mode;
		layout.attributeCount = (u32) attribs.size();
		layout.attributes     = attribs.data();
		all_layouts.push_back(layout);
	}

	WGPURenderPipelineDescriptor p = WGPU_RENDER_PIPELINE_DESCRIPTOR_INIT;

	p.layout = m_layout;

	// Each sequence of 3 vertices is considered as a triangle
	p.primitive.topology = WGPUPrimitiveTopology_TriangleList;

	// We'll see later how to specify the order in which vertices should be
	// connected. When not specified, vertices are considered sequentially.
	p.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;

	// The face orientation is defined by assuming that when looking
	// from the front of the face, its corner vertices are enumerated
	// in the counter-clockwise (CCW) order.
	p.primitive.frontFace = WGPUFrontFace_CCW;

	// But the face orientation does not matter much because we do not
	// cull (i.e. "hide") the faces pointing away from us (which is often
	// used for optimization).
	p.primitive.cullMode = WGPUCullMode_None;

	// Vertex state
	WGPUVertexState vs = WGPU_VERTEX_STATE_INIT;
	vs.module = m_spec.shader.module();
	vs.entryPoint = to_wgpu_string_view(m_spec.shader.vertex_entry);
	vs.bufferCount = (u32) all_layouts.size();
	vs.buffers     = all_layouts.data();

	WGPUColorTargetState color_target_state = WGPU_COLOR_TARGET_STATE_INIT;
	color_target_state.format = m_spec.surface_format;
	WGPUBlendState blend_state = WGPU_BLEND_STATE_INIT;
	color_target_state.blend = &blend_state;

	// Fragment state
	WGPUFragmentState fs = WGPU_FRAGMENT_STATE_INIT;
	fs.module = m_spec.shader.module();
	fs.entryPoint = to_wgpu_string_view(m_spec.shader.fragment_entry);
	fs.targetCount = 1;
	fs.targets = &color_target_state;

	p.vertex = vs;
	p.fragment = &fs;

	m_pipeline = wgpuDeviceCreateRenderPipeline(
		m_context.get_native_device(), 
		&p
	);

}



}
