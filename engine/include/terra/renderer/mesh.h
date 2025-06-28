#pragma once

#include "terra/core/base.h"
#include "terra/renderer/buffer.h"

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // for glm::translate, rotate, scale


namespace terra {

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
};

struct Transform {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f); // Euler
    glm::vec3 scale    = glm::vec3(1.0f);

    glm::mat4 get_model_matrix() const {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
        glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1, 0, 0));
        glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0, 1, 0));
        glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0, 0, 1));
        glm::mat4 rotationMatrix = rotZ * rotY * rotX;
        glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);
        return translation * rotationMatrix * scaleMatrix;
    }
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

    // 🔧 New methods
    Transform& get_transform() { return m_transform; }
    const Transform& get_transform() const { return m_transform; }
    glm::mat4 get_model_matrix() const { return m_transform.get_model_matrix(); }

private:
    VertexBuffer m_vertex_buffer;
    IndexBuffer m_index_buffer;
    u32 m_index_count = 0;
    u32 m_vertex_count = 0;

    // 🔧 New member
    Transform m_transform;
};

} // namespace terra 
