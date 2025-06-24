#include "terrapch.h"
#include "terra/renderer/mesh.h"
#include "terra/renderer/renderer_api.h"

namespace terra {

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices) {
    auto& context = RendererAPI::get_context();
    m_vertex_buffer = Buffer::create_vertex_buffer(context, vertices.data(), vertices.size() * sizeof(Vertex), 0, "VertexBuffer");
    m_index_buffer = Buffer::create_index_buffer(context, indices.data(), indices.size() * sizeof(u32), WGPUIndexFormat_Uint32, "IndexBuffer");
    m_index_count = indices.size();
}

Mesh::~Mesh() {
    if (m_vertex_buffer.buffer) wgpuBufferRelease(m_vertex_buffer.buffer);
    if (m_index_buffer.buffer)  wgpuBufferRelease(m_index_buffer.buffer);
}

} // namespace terra 
