#include "terra/core/assert.h"
#include "terra/core/logger.h"
#include "terrapch.h"

#include "terra/resources/resource_manager.h"
#include "terra/renderer/renderer_command.h"
#include "terra/renderer/renderer.h"
#include "terra/renderer/material.h"

namespace terra {

Renderer::Renderer(WebGPUContext& ctx)
    : m_context(ctx)
    , m_queue(*ctx.get_queue())
{}

Renderer::~Renderer() {
    if (m_vertex_buffer.buffer) wgpuBufferRelease(m_vertex_buffer.buffer);
    if (m_index_buffer.buffer)  wgpuBufferRelease(m_index_buffer.buffer);
}

// Allocate static context
void Renderer::init() {
    std::vector<f32> vertex_data;
    std::vector<u32> index_data;

    if (!ResourceManager::load_geometry("objects/webgpu.txt", vertex_data, index_data)) {
        TR_CORE_ASSERT(false, "Failed to load geometry");
        return;
    }

    m_vertex_buffer = Buffer::create_vertex_buffer(
        m_context,
        vertex_data.data(),
        vertex_data.size() * sizeof(f32),
        0,
        "Vertex Buffer"
    );

    m_index_buffer = Buffer::create_index_buffer(
        m_context,
        index_data.data(),
        index_data.size() * sizeof(u32),
        WGPUIndexFormat_Uint32,
        "Index Buffer"
    );

    ref<Shader> shader = create_ref<Shader>(
        Shader::from_file(
            m_context, 
            "shaders/shader.wgsl", 
            "Triangle Shader Module"
        )
    );
    shader->vertex_entry = "vs_main";
    shader->fragment_entry = "fs_main";

    // Create a material directly
    auto material = create_ref<Material>(m_context, "BasicMaterial");
    material->set_shader(shader);

    // Define material parameters
    material->define_parameter(
        "u_time", 
        0, 
        MaterialParamType::Float, 
        WGPUShaderStage_Vertex | WGPUShaderStage_Fragment
    );

    // Set default values
    material->set_default_float("u_time", 0.0f);

    PipelineSpecification spec;
    spec.shader = shader;
    spec.surface_format = m_context.get_preferred_format();

    VertexBufferLayoutSpec vb;
    vb.stride       = (2 + 3) * sizeof(f32);
    vb.step_mode    = WGPUVertexStepMode_Vertex;
    vb.attributes = {
        { 0, WGPUVertexFormat_Float32x2, 0 },
        { 1, WGPUVertexFormat_Float32x3, 2 * sizeof(f32) },
    };

    spec.vertex_buffers.push_back(vb);

    m_index_count = (u32) index_data.size();

    UniformSpec time_uniform;
    time_uniform.binding = 0;
    time_uniform.size = sizeof(f32);
    time_uniform.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;

    spec.uniforms.push_back(time_uniform);

    m_pipeline = create_scope<Pipeline>(m_context, spec);

    // Create material instance from the material template
    m_material_instance = material->create_instance(m_pipeline.get());
}

void Renderer::update_uniforms(f32 time) {
    // Use the material instance to update the time uniform
    m_material_instance->set_parameter_float("u_time", time);
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
        m_vertex_buffer.buffer, 
        0, 
        m_vertex_buffer.size
    );

    wgpuRenderPassEncoderSetIndexBuffer(
        m_current_pass,
        m_index_buffer.buffer,
        WGPUIndexFormat_Uint32,
        0,
        m_index_buffer.size
    );

    // Bind the material instance (which contains the uniforms)
    m_material_instance->bind(m_current_pass);

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
