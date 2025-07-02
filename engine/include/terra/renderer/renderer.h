#pragma once

#include "terra/core/base.h"
#include "terra/core/timestep.h"
#include "terra/renderer/buffer.h"
#include "terra/renderer/pipeline.h"
#include "terra/renderer/material_instance.h"
#include "terra/renderer/camera.h"
#include "terra/renderer/mesh.h"
#include "terra/renderer/render_pass.h"

namespace terra {

class WebGPUContext;
class CommandQueue;

struct RendererStats {
    u32 draw_calls = 0;
    u32 mesh_count = 0;
    u32 vertex_count = 0;
    u32 index_count = 0;

    f32 frame_time_ms = 0.0f;
    f32 fps = 0.0f;

    void reset() {
        draw_calls = 0;
        mesh_count = 0;
        vertex_count = 0;
        index_count = 0;
    }
};

class Renderer {
public:
    explicit Renderer(WebGPUContext& context);
    ~Renderer();

    void init();

    // call once per frame
    void begin_frame();
    void end_frame();
    
    void begin_scene(const Camera& camera);
    void end_scene();

    void submit(
        const ref<Mesh>& mesh,
        const ref<MaterialInstance>& material,
        const void* instance,
        u32 size,
        u32 binding,
        u32 group
    );

    void begin_ui_pass();
    void end_ui_pass();

    void draw_scene();
    void draw_ui();

    void invalidate_surface_view();


    // high‐level draws
    void clear_color(f32 r, f32 g, f32 b, f32 a);

    void on_resize(u32 width, u32 height);

    // RenderPass management
    RenderPass* create_render_pass(const RenderPassDesc& desc);
    const std::vector<std::unique_ptr<RenderPass>>& get_render_passes() const { return m_render_passes; }

    u64 create_pipeline(const PipelineSpecification& spec);
    ref<Pipeline> get_pipeline(u64 id) const;


    wgpu::RenderPassEncoder get_current_pass_encoder() const { return m_current_pass; }
    
    const RendererStats& get_stats() const { return m_stats; }
    RendererStats& get_stats_mutable() { return m_stats; }

private:
    struct SceneData {
        const Camera* camera;
    };
    scope<SceneData> m_scene_data;

    struct DrawBatch {
        ref<Mesh> mesh;
        ref<MaterialInstance> material;

        std::vector<u8> instance_data;  // raw blob
        u32 instance_stride = 0;
        u32 instance_count  = 0;
        u32 binding = 0;
        u32 group = 1;

        wgpu::Buffer instance_buffer;
        u64       buffer_capacity = 0;
    };

    std::vector<DrawBatch> m_draw_batches;

    WebGPUContext&   m_context;
    CommandQueue&    m_queue;

    wgpu::TextureView    m_target_texture_view{};

    wgpu::TextureView    m_depth_texture_view{};
    wgpu::TextureFormat  m_depth_texture_format = wgpu::TextureFormat::Depth24Plus;

    wgpu::RenderPassEncoder m_current_pass = nullptr;

    std::vector<scope<RenderPass>> m_render_passes;

    RendererStats m_stats;

    std::unordered_map<u64, ref<Pipeline>> m_pipeline_cache;
    u64 m_next_pipeline_id = 1;

    wgpu::Color m_clear_color;

};

}
