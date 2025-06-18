#include "terra/core/base.h"
#include "terrapch.h"
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

static const char* shader_source = R"(
struct VertexInput {
    @location(0) position: vec2f,
    @location(1) color:    vec3f,
};

struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0)        color:    vec3f,
};

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    // lift 2d → 4d position:
    out.position = vec4f(in.position, 0.0, 1.0);
    // just forward the color
    out.color    = in.color;
    return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    // drop back out as 4d with alpha=1
    return vec4f(in.color, 1.0);
}
)";


// Allocate static context
void Renderer::init() {

    std::vector<f32> vertex_data = {
        // x,   y,     r,   g,   b
        -0.5, -0.5,   1.0, 0.0, 0.0,
        +0.5, -0.5,   0.0, 1.0, 0.0,
        +0.5, +0.5,   0.0, 0.0, 1.0,
        -0.5, +0.5,   1.0, 1.0, 0.0
    };

    std::vector<u16> index_data = {
        0, 1, 2, // Triangle #0 connects points #0, #1 and #2
        0, 2, 3  // Triangle #1 connects points #0, #2 and #3
    };
    
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
        index_data.size() * sizeof(u16),
        WGPUBufferUsage_Index | WGPUBufferUsage_CopyDst,
        "Index Buffer"
    );

    Shader shader = Shader::create_from_wgsl(
        m_context, 
        shader_source, 
        "Triangle Shader Module"
    );
    shader.vertex_entry = "vs_main";
    shader.fragment_entry = "fs_main";

    PipelineSpecification spec(std::move(shader));
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

    u32 nums_per_vertex = (u32) (spec.vertex_buffers[0].stride / sizeof(f32));
    m_vertex_count = (u32) vertex_data.size() / nums_per_vertex;

    m_index_count = (u32) index_data.size();

    m_pipeline = create_scope<Pipeline>(m_context, std::move(spec));
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


void Renderer::clear_color(float r, float g, float b, float a) {
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
        WGPUIndexFormat_Uint16, 
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
