#include "terrapch.h"
#include "terra/renderer/renderer_api.h"
#include "terra/renderer/renderer.h"

namespace terra {

scope<Renderer> RendererAPI::s_renderer = nullptr;
scope<RendererAPI::RendererAPIData> RendererAPI::s_data = create_scope<RendererAPI::RendererAPIData>();

void RendererAPI::init(WebGPUContext* context) {
    s_data->context = context;
    s_renderer = create_scope<Renderer>(*s_data->context);
    s_renderer->init();
}

void RendererAPI::shutdown() {
    s_renderer.reset();
}

void RendererAPI::begin_frame() {
    s_renderer->begin_frame();
}

void RendererAPI::end_frame() {
    s_renderer->end_frame();
}

void RendererAPI::begin_ui_pass() {
    s_renderer->begin_ui_pass();
}

void RendererAPI::end_ui_pass() {
    s_renderer->end_ui_pass();
}

void RendererAPI::clear_color(f32 r, f32 g, f32 b, f32 a)
{
    s_renderer->clear_color(r, g, b, a);
}

ref<Shader> RendererAPI::create_shader(const std::string& path, const std::string& label) {
    return create_ref<Shader>(Shader::from_file(*s_data->context, path, label));
}

ref<Material> RendererAPI::create_material(const std::string& name, const ref<Shader>& shader) {
    auto mat = create_ref<Material>(*s_data->context, name);
    mat->set_shader(shader);
    return mat;
}

ref<Mesh> RendererAPI::create_mesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices) {
    return create_ref<Mesh>(vertices, indices);
}

void RendererAPI::begin_scene(const Camera& camera) {
    s_renderer->begin_scene(camera);
}

void RendererAPI::end_scene() {
    s_renderer->end_scene();
}

WebGPUContext& RendererAPI::get_context() {
    return *s_data->context;
}

wgpu::RenderPassEncoder RendererAPI::get_current_pass_encoder() {
    return s_renderer->get_current_pass_encoder();
}

const RendererStats& RendererAPI::get_stats() {
    return s_renderer->get_stats();
}

RendererStats& RendererAPI::get_stats_mutable() {
    return s_renderer->get_stats_mutable();
}

u64 RendererAPI::create_pipeline(const PipelineSpecification& spec) {
    return s_renderer->create_pipeline(spec);
}

ref<Pipeline> RendererAPI::get_pipeline(u64 pipeline_id) {
    return s_renderer->get_pipeline(pipeline_id);
}

} 
