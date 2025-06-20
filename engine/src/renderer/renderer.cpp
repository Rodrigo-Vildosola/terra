#include "terra/core/assert.h"
#include "terra/core/logger.h"
#include "terrapch.h"

#include "terra/resources/resource_manager.h"
#include "terra/renderer/renderer_command.h"
#include "terra/renderer/renderer.h"

namespace terra {

Renderer::Renderer(WebGPUContext& ctx)
    : m_context(ctx)
    , m_queue(*ctx.get_queue())
{}

Renderer::~Renderer() {
    if (m_vertex_buffer) wgpuBufferRelease(m_vertex_buffer);

}

// Allocate static context
void Renderer::init() {
    std::vector<f32> vertex_data;
    std::vector<u32> index_data;

    if (!ResourceManager::load_geometry("objects/webgpu.txt", vertex_data, index_data)) {
        TR_CORE_ASSERT(false, "Failed to load geometry");
        return;
    }

    m_vertex_buffer = Buffer::create(
        m_context.get_native_device(),
        m_queue.get_native_queue(),
        vertex_data.data(),
        vertex_data.size() * sizeof(f32),
        WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst,
        "Vertex Buffer"
    );

    m_index_buffer = Buffer::create(
        m_context.get_native_device(),
        m_queue.get_native_queue(),
        index_data.data(),
        index_data.size() * sizeof(u32),
        WGPUBufferUsage_Index | WGPUBufferUsage_CopyDst,
        "Index Buffer"
    );


    Shader shader = Shader::from_file(
        m_context, 
        "shaders/shader.wgsl", 
        "Triangle Shader Module"
    );
    shader.vertex_entry = "vs_main";
    shader.fragment_entry = "fs_main";

    PipelineSpecification spec;
    spec.shader = &shader;
    spec.surface_format = m_context.get_preferred_format();
    spec.uniform_buffer_size = 0;

    VertexBufferLayoutSpec vb;
    vb.stride       = (2 + 3) * sizeof(f32);
    vb.step_mode    = WGPUVertexStepMode_Vertex;
    vb.attributes = {
        { 0, WGPUVertexFormat_Float32x2, 0 },
        { 1, WGPUVertexFormat_Float32x3, 2 * sizeof(f32) },
    };

    spec.vertex_buffers.push_back(vb);

    m_index_count = (u32) index_data.size();

    m_pipeline = create_scope<Pipeline>(m_context, spec);
}

void Renderer::init_bind_group() {
    WGPUBindGroupEntry binding = WGPU_BIND_GROUP_ENTRY_INIT;

	// The index of the binding (the entries in bindGroupDesc can be in any order)
	binding.binding = 0;
	// The buffer it is actually bound to
	binding.buffer = m_uniform_buffer;
    // We can specify an offset within the buffer, so that a single buffer can hold
	// multiple uniform blocks.
	binding.offset = 0;
	// And we specify again the size of the buffer.
	binding.size = 4 * sizeof(f32);

    // A bind group contains one or multiple bindings
	WGPUBindGroupDescriptor bind_group_desc = WGPU_BIND_GROUP_DESCRIPTOR_INIT;
	// There must be as many bindings as declared in the layout!
	bind_group_desc.entryCount = 1;
	bind_group_desc.entries = &binding;
	m_bind_group = wgpuDeviceCreateBindGroup(
        m_context.get_native_device(), 
        &bind_group_desc
    );
	
}

void Renderer::begin_frame() {
    std::tie(m_surface_texture, m_target_view) = m_context.get_next_surface_view();
}

void Renderer::begin_scene_pass() {
    // clears to clearColor
    m_current_pass = RendererCommand::begin_render_pass(
        m_queue, 
        m_target_view, 
        WGPULoadOp_Clear
    );
}

void Renderer::end_scene_pass() {
    RendererCommand::end_render_pass(m_queue);
    m_current_pass = nullptr;
}

void Renderer::begin_ui_pass() {
    // load what we just drew, draw UI on top
    m_current_pass = RendererCommand::begin_render_pass(
        m_queue, 
        m_target_view, 
        WGPULoadOp_Load
    );
}

void Renderer::end_ui_pass() {
    RendererCommand::end_render_pass(m_queue);
    m_current_pass = nullptr;
}

void Renderer::end_frame() {
    m_context.swap_buffers();
    m_queue.poll(false);
}


void Renderer::clear_color(f32 r, f32 g, f32 b, f32 a) {
    RendererCommand::set_clear_color(r, g, b, a);
}

void Renderer::draw() {
    if (!m_current_pass) return;

    m_pipeline->bind(m_current_pass); // this does setPipeline()

    // draw 3 vertices as a triangle
    wgpuRenderPassEncoderSetVertexBuffer(
        m_current_pass, 
        0, 
        m_vertex_buffer, 
        0, 
        wgpuBufferGetSize(m_vertex_buffer)
    );

    wgpuRenderPassEncoderSetIndexBuffer(
        m_current_pass,
        m_index_buffer,
        WGPUIndexFormat_Uint32,
        0,
        wgpuBufferGetSize(m_index_buffer)
    );

    wgpuRenderPassEncoderDrawIndexed(
        m_current_pass, 
        m_index_count, 
        1, 
        0, 
        0,
        0
    );
}


}
