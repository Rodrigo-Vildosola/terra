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
	// if (m_pipeline) wgpuRenderPipelineRelease(m_pipeline);
	// if (m_layout) wgpuPipelineLayoutRelease(m_layout);
	// if (m_bind_group_layout) wgpuBindGroupLayoutRelease(m_bind_group_layout);
}

void Pipeline::bind(wgpu::RenderPassEncoder render_pass) const {
    if (!m_pipeline) {
        TR_CORE_ERROR("Tried to bind a null pipeline!");
        return;
    }

	render_pass.SetPipeline(m_pipeline);
}


void Pipeline::create_pipeline(const PipelineSpecification& spec) {
	const auto& device = m_context.get_native_device();

	std::vector<wgpu::VertexAttribute> 		flat_attributes;
	std::vector<wgpu::VertexBufferLayout> 	all_layouts;
	flat_attributes.reserve(spec.vertex_buffers.size());
	all_layouts.reserve(spec.vertex_buffers.size());

	for (const VertexBufferLayoutSpec& vb : spec.vertex_buffers) {
		wgpu::VertexBufferLayout layout = {};
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
		std::vector<wgpu::BindGroupLayoutEntry> layout_entries;
		for (const auto& uniform : spec.uniforms) {
			wgpu::BindGroupLayoutEntry entry = {};
			entry.binding = uniform.binding;
			entry.visibility = uniform.visibility;
			entry.buffer.type = wgpu::BufferBindingType::Uniform;
			entry.buffer.minBindingSize = uniform.size;
			layout_entries.push_back(entry);
		}

		wgpu::BindGroupLayoutDescriptor bgl_desc = {};
		bgl_desc.entryCount = (u32) layout_entries.size();
		bgl_desc.entries = layout_entries.data();

		wgpu::BindGroupLayout bind_group_layout = device.CreateBindGroupLayout(&bgl_desc);

		// Append the layout to the final pipeline layout
		m_bind_group_layouts.push_back(bind_group_layout);
	}

	if (!spec.storages.empty()) {
		std::vector<wgpu::BindGroupLayoutEntry> entries;
		for (auto& sb : spec.storages) {
			wgpu::BindGroupLayoutEntry e{};
			e.binding       = sb.binding;
			e.visibility    = sb.visibility;
			e.buffer.type   = wgpu::BufferBindingType::ReadOnlyStorage;
			e.buffer.minBindingSize = sb.size;
			entries.push_back(e);
		}

		wgpu::BindGroupLayoutDescriptor bgl_desc = {};
		bgl_desc.entryCount = (u32) entries.size();
		bgl_desc.entries = entries.data();

		wgpu::BindGroupLayout bind_group_layout = device.CreateBindGroupLayout(&bgl_desc);

		// create group-1 layout from entries
		m_bind_group_layouts.push_back(bind_group_layout);
	}

	wgpu::PipelineLayoutDescriptor layout_desc = {};
    layout_desc.bindGroupLayoutCount = (u32) m_bind_group_layouts.size();
    layout_desc.bindGroupLayouts     = m_bind_group_layouts.data();

    m_layout = device.CreatePipelineLayout(&layout_desc);

	wgpu::RenderPipelineDescriptor p = {};

	p.layout = m_layout;

	// Each sequence of 3 vertices is considered as a triangle
	p.primitive.topology = wgpu::PrimitiveTopology::TriangleList;

	// We'll see later how to specify the order in which vertices should be
	// connected. When not specified, vertices are considered sequentially.
	p.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;

	// The face orientation is defined by assuming that when looking
	// from the front of the face, its corner vertices are enumerated
	// in the counter-clockwise (CCW) order.
	p.primitive.frontFace = wgpu::FrontFace::CCW;

	// But the face orientation does not matter much because we do not
	// cull (i.e. "hide") the faces pointing away from us (which is often
	// used for optimization).
	p.primitive.cullMode = wgpu::CullMode::None;


	// Vertex state
	wgpu::VertexState vs = {};
	vs.module = spec.shader->module();
	vs.entryPoint = to_wgpu_string_view(spec.shader->vertex_entry);
	vs.bufferCount = (u32) all_layouts.size();
	vs.buffers     = all_layouts.data();

	wgpu::ColorTargetState color_target_state = {};
	color_target_state.format = spec.surface_format;
	wgpu::BlendState blend_state = {};
	color_target_state.blend = &blend_state;

	// Fragment state
	wgpu::FragmentState fs = {};
	fs.module = spec.shader->module();
	fs.entryPoint = to_wgpu_string_view(spec.shader->fragment_entry);
	fs.targetCount = 1;
	fs.targets = &color_target_state;

	p.vertex = vs;
	p.fragment = &fs;

	wgpu::DepthStencilState depth_stencil = {};
	if (spec.depth_view) {
		depth_stencil.format = spec.depth_format;
		depth_stencil.depthWriteEnabled = wgpu::OptionalBool::True;
		depth_stencil.depthCompare = wgpu::CompareFunction::Less;

		// depth_stencil.stencilFront = wgpu::StencilFaceState{
		// 	.compare = wgpu::CompareFunction_Always,
		// 	.failOp = wgpu::StencilOperation_Keep,
		// 	.depthFailOp = wgpu::StencilOperation_Keep,
		// 	.passOp = wgpu::StencilOperation_Keep
		// };
		// depth_stencil.stencilBack = depth_stencil.stencilFront;
		// depth_stencil.stencilReadMask = 0xFFFFFFFF;
		// depth_stencil.stencilWriteMask = 0xFFFFFFFF;

		p.depthStencil = &depth_stencil;

		TR_CORE_CRITICAL("Creating the depth stencil state");
	} else {
		p.depthStencil = nullptr;
	}

	m_pipeline = device.CreateRenderPipeline(&p);

}



}
