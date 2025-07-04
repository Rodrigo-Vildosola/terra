#pragma once

#include "terra/core/base.h"
#include "terra/renderer/buffer.h"
#include "terra/renderer/pipeline_specification.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // for glm::translate, rotate, scale


namespace terra {

struct MeshSpecification {
    const void* vertex_data = nullptr;
    u32 vertex_count = 0;

    const u32* index_data = nullptr;
    u32 index_count = 0;

    VertexBufferLayoutSpec layout;

    std::string_view debug_name = "Unnamed Mesh";
};

class Mesh {
public:
    // Mesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices);
    Mesh(const MeshSpecification& spec);
    ~Mesh() = default;

    const VertexBuffer& get_vertex_buffer() const { return m_vertex_buffer; }
    const IndexBuffer& get_index_buffer() const { return m_index_buffer; }
    static VertexBufferLayoutSpec get_default_layout();

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
