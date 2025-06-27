#pragma once

#include "terra/core/base.h"
#include "terra/renderer/buffer.h"

#include <vector>
#include <glm/glm.hpp>

namespace terra {

struct Vertex {
    glm::vec2 position;
    glm::vec3 color;
};

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices);
    ~Mesh();

    const VertexBuffer& get_vertex_buffer() const { return m_vertex_buffer; }
    const IndexBuffer& get_index_buffer() const { return m_index_buffer; }

    u32 get_index_count() const { return m_index_count; }
    u32 get_vertex_count() const { return m_vertex_count; }

    static ref<Mesh> from_file(const std::filesystem::path& path);

private:
    VertexBuffer m_vertex_buffer;
    IndexBuffer m_index_buffer;
    u32 m_index_count = 0;
    u32 m_vertex_count = 0;
};

} // namespace terra 
