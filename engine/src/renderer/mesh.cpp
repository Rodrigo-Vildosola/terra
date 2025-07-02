#include "terrapch.h"
#include "terra/renderer/mesh.h"
#include "terra/renderer/renderer_api.h"
#include "terra/resources/resource_manager.h"

namespace terra {

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices) {
    auto& context = RendererAPI::get_context();
    m_vertex_buffer = Buffer::create_vertex_buffer(context, vertices.data(), vertices.size() * sizeof(Vertex), 0, "VertexBuffer");
    m_index_buffer = Buffer::create_index_buffer(context, indices.data(), indices.size() * sizeof(u32), wgpu::IndexFormat::Uint32, "IndexBuffer");
    m_index_count = indices.size();
    m_vertex_count = vertices.size();
}

Mesh::~Mesh() {
    // if (m_vertex_buffer.buffer) wgpuBufferRelease(m_vertex_buffer.buffer);
    // if (m_index_buffer.buffer)  wgpuBufferRelease(m_index_buffer.buffer);
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

    std::vector<Vertex> vertices;
    vertices.reserve(raw_vertex_data.size() / 6);

    for (size_t i = 0; i + 5 < raw_vertex_data.size(); i += 6) {
        Vertex v;
        v.position = { raw_vertex_data[i + 0], raw_vertex_data[i + 1], raw_vertex_data[i + 2] };
        v.color    = { raw_vertex_data[i + 3], raw_vertex_data[i + 4], raw_vertex_data[i + 5] };
        vertices.push_back(v);
    }

    return create_ref<Mesh>(vertices, index_data);
}


} // namespace terra 
