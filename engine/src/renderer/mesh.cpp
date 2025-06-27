#include "terrapch.h"
#include "terra/renderer/mesh.h"
#include "terra/renderer/renderer_api.h"
#include "terra/resources/resource_manager.h"

namespace terra {

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices) {
    auto& context = RendererAPI::get_context();
    m_vertex_buffer = Buffer::create_vertex_buffer(context, vertices.data(), vertices.size() * sizeof(Vertex), 0, "VertexBuffer");
    m_index_buffer = Buffer::create_index_buffer(context, indices.data(), indices.size() * sizeof(u32), WGPUIndexFormat_Uint32, "IndexBuffer");
    m_index_count = indices.size();
    m_vertex_count = vertices.size();
}

Mesh::~Mesh() {
    if (m_vertex_buffer.buffer) wgpuBufferRelease(m_vertex_buffer.buffer);
    if (m_index_buffer.buffer)  wgpuBufferRelease(m_index_buffer.buffer);
}

ref<Mesh> Mesh::from_file(const std::filesystem::path& path) {
    std::vector<f32> raw_vertex_data;
    std::vector<u32> index_data;

    if (!ResourceManager::load_geometry(path, raw_vertex_data, index_data)) {
        TR_CORE_ERROR("Mesh::from_file failed: {}", path.string());
        return nullptr;
    }

    std::vector<Vertex> vertices;
    for (size_t i = 0; i + 4 < raw_vertex_data.size(); i += 5) {
        Vertex v;
        v.position = { raw_vertex_data[i + 0], raw_vertex_data[i + 1] };
        v.color    = { raw_vertex_data[i + 2], raw_vertex_data[i + 3], raw_vertex_data[i + 4] };
        vertices.push_back(v);
    }

    return create_ref<Mesh>(vertices, index_data);
}

} // namespace terra 
