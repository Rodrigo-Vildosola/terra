#include "terrapch.h"
#include "terra/renderer/mesh.h"
#include "terra/renderer/renderer_api.h"
#include "terra/resources/resource_manager.h"

namespace terra {

// Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices) {
//     auto& ctx = RendererAPI::get_context();

//     m_vertex_buffer = Buffer::create_vertex_buffer(
//         ctx, 
//         vertices.data(), 
//         vertices.size() * sizeof(Vertex), 
//         0, 
//         "VertexBuffer"
//     );
//     m_index_buffer = Buffer::create_index_buffer(
//         ctx, 
//         indices.data(), 
//         indices.size() * sizeof(u32),
//         wgpu::IndexFormat::Uint32, 
//         "IndexBuffer"
//     );

//     m_index_count = indices.size();
//     m_vertex_count = vertices.size();
// }

Mesh::Mesh(const MeshSpecification& spec) {
    auto& ctx = RendererAPI::get_context();

    m_vertex_buffer = Buffer::create_vertex_buffer(
        ctx, 
        spec.vertex_data, 
        spec.vertex_count * spec.layout.stride, 
        0, 
        spec.debug_name.data()
    );

    m_index_buffer  = Buffer::create_index_buffer(
        ctx, 
        spec.index_data, 
        spec.index_count * sizeof(u32), 
        wgpu::IndexFormat::Uint32, 
        spec.debug_name.data()
    );

    m_vertex_count = spec.vertex_count;
    m_index_count = spec.index_count;
}




ref<Mesh> Mesh::from_file(const std::filesystem::path& path) {
    std::vector<f32> raw_vertex_data;
    std::vector<u32> index_data;

    if (!ResourceManager::load_geometry(path, raw_vertex_data, index_data, true)) {
        TR_CORE_ERROR("Mesh::from_file failed: {}", path.string());
        return nullptr;
    }

    if (raw_vertex_data.size() % 6 != 0) {
        TR_CORE_ERROR("Invalid vertex format: expected 6 floats per vertex (x, y, z, r, g, b)");
        return nullptr;
    }

    MeshSpecification spec;
    spec.vertex_data = raw_vertex_data.data();
    spec.vertex_count = raw_vertex_data.size() / 6;
    spec.index_data = index_data.data();
    spec.index_count = index_data.size();
    spec.layout = get_default_layout();
    spec.debug_name = path.filename().string();

    return create_ref<Mesh>(spec);
}

VertexBufferLayoutSpec Mesh::get_default_layout() {

    VertexBufferLayoutSpec layout;
    layout.stride = sizeof(f32) * 6;
    layout.step_mode = wgpu::VertexStepMode::Vertex;
    layout.attributes = {
        { 0, wgpu::VertexFormat::Float32x3, 0 },
        { 1, wgpu::VertexFormat::Float32x3, sizeof(f32) * 3 },
    };

    return layout;
}


} // namespace terra 
