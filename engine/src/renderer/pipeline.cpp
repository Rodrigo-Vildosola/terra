#include "terra/renderer/pipeline.h"
#include "terra/core/logger.h"
#include "terra/helpers/error.h"
#include "terra/helpers/string.h"
#include "terra/helpers/user_data.h"
#include "terra/renderer/pipeline_specification.h"


namespace terra {

Pipeline::Pipeline(WebGPUContext& context, const PipelineSpecification& spec)
    : m_context(context), m_spec(spec) {
    create_pipeline(spec);
}

Pipeline::~Pipeline() {
	if (m_pipeline) wgpuRenderPipelineRelease(m_pipeline);
	if (m_layout) wgpuPipelineLayoutRelease(m_layout);
	// if (m_bind_group_layout) wgpuBindGroupLayoutRelease(m_bind_group_layout);
}

void Pipeline::bind(WGPURenderPassEncoder render_pass) const {
    if (!m_pipeline) {
        TR_CORE_ERROR("Tried to bind a null pipeline!");
        return;
    }

    wgpuRenderPassEncoderSetPipeline(render_pass, m_pipeline);
}


void Pipeline::create_pipeline(const PipelineSpecification& spec) {
	std::vector<WGPUVertexAttribute> flat_attributes;
	std::vector<WGPUVertexBufferLayout>            all_layouts;
	flat_attributes.reserve(spec.vertex_buffers.size());
	all_layouts.reserve(spec.vertex_buffers.size());

	for (const VertexBufferLayoutSpec& vb : spec.vertex_buffers) {
		WGPUVertexBufferLayout layout = WGPU_VERTEX_BUFFER_LAYOUT_INIT;
		layout.arrayStride = vb.stride;
		layout.stepMode = vb.step_mode;

		u64 attribute_offset = flat_attributes.size();
		for (const auto& a : vb.attributes) {
			flat_attributes.push_back({
				.shaderLocation = a.shader_location,
				.format = a.format,
				.offset = a.offset,
			});
		}

		layout.attributeCount = static_cast<u32>(vb.attributes.size());
		layout.attributes = flat_attributes.data() + attribute_offset;

		all_layouts.push_back(layout);
	}

	if (!spec.uniforms.empty()) {
		std::vector<WGPUBindGroupLayoutEntry> layout_entries;
		for (const auto& uniform : spec.uniforms) {
			WGPUBindGroupLayoutEntry entry = WGPU_BIND_GROUP_LAYOUT_ENTRY_INIT;
			entry.binding = uniform.binding;
			entry.visibility = uniform.visibility;
			entry.buffer.type = WGPUBufferBindingType_Uniform;
			entry.buffer.minBindingSize = uniform.size;
			layout_entries.push_back(entry);
		}

		WGPUBindGroupLayoutDescriptor bgl_desc = WGPU_BIND_GROUP_LAYOUT_DESCRIPTOR_INIT;
		bgl_desc.entryCount = static_cast<u32>(layout_entries.size());
		bgl_desc.entries = layout_entries.data();

		WGPUBindGroupLayout bind_group_layout = wgpuDeviceCreateBindGroupLayout(m_context.get_native_device(), &bgl_desc);

		// Append the layout to the final pipeline layout
		m_bind_group_layouts.push_back(bind_group_layout);
	}

	WGPUPipelineLayoutDescriptor layout_desc = WGPU_PIPELINE_LAYOUT_DESCRIPTOR_INIT;
    layout_desc.bindGroupLayoutCount = (u32) m_bind_group_layouts.size();
    layout_desc.bindGroupLayouts     = m_bind_group_layouts.data();

    m_layout = wgpuDeviceCreatePipelineLayout(
    	m_context.get_native_device(),
		&layout_desc
    );

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
	vs.module = spec.shader->module();
	vs.entryPoint = to_wgpu_string_view(spec.shader->vertex_entry);
	vs.bufferCount = (u32) all_layouts.size();
	vs.buffers     = all_layouts.data();

	WGPUColorTargetState color_target_state = WGPU_COLOR_TARGET_STATE_INIT;
	color_target_state.format = spec.surface_format;
	WGPUBlendState blend_state = WGPU_BLEND_STATE_INIT;
	color_target_state.blend = &blend_state;

	// Fragment state
	WGPUFragmentState fs = WGPU_FRAGMENT_STATE_INIT;
	fs.module = spec.shader->module();
	fs.entryPoint = to_wgpu_string_view(spec.shader->fragment_entry);
	fs.targetCount = 1;
	fs.targets = &color_target_state;

	p.vertex = vs;
	p.fragment = &fs;

	WGPUDepthStencilState depth_stencil = WGPU_DEPTH_STENCIL_STATE_INIT;
	if (spec.depth_view) {
		depth_stencil.format = spec.depth_format;
		depth_stencil.depthWriteEnabled = WGPUOptionalBool_True;
		depth_stencil.depthCompare = WGPUCompareFunction_Less;

		// depth_stencil.stencilFront = WGPUStencilFaceState{
		// 	.compare = WGPUCompareFunction_Always,
		// 	.failOp = WGPUStencilOperation_Keep,
		// 	.depthFailOp = WGPUStencilOperation_Keep,
		// 	.passOp = WGPUStencilOperation_Keep
		// };
		// depth_stencil.stencilBack = depth_stencil.stencilFront;
		// depth_stencil.stencilReadMask = 0xFFFFFFFF;
		// depth_stencil.stencilWriteMask = 0xFFFFFFFF;

		p.depthStencil = &depth_stencil;

		TR_CORE_CRITICAL("Creating the depth stencil state");
	} else {
		p.depthStencil = nullptr;
	}

	m_pipeline = wgpuDeviceCreateRenderPipeline(
		m_context.get_native_device(), 
		&p
	);

}



}
