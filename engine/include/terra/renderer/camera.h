#pragma once

#include <glm/glm.hpp>

namespace terra {

class Camera {
public:
    Camera() = default;
    virtual ~Camera() = default;

    const glm::mat4& get_projection_matrix() const { return m_projection_matrix; }
    const glm::mat4& get_view_matrix() const { return m_view_matrix; }

protected:
    glm::mat4 m_projection_matrix{1.0f};
    glm::mat4 m_view_matrix{1.0f};
};

} 
