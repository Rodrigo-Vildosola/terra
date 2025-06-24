#pragma once

#include "terrapch.h"
#include "terra/renderer/camera.h"

namespace terra {

class OrthographicCamera : public Camera
{
public:
    OrthographicCamera(f32 left, f32 right, f32 bottom, f32 top);

    void set_projection(f32 left, f32 right, f32 bottom, f32 top);

    const glm::vec3& get_position() const { return m_position; }
    void set_position(const glm::vec3& position) { m_position = position; update_view_matrix(); }

    f32 get_rotation() const { return m_rotation; }
    void set_rotation(f32 rotation) { m_rotation = rotation; update_view_matrix(); }

private:
    void update_view_matrix();

    glm::vec3 m_position = { 0.0f, 0.0f, 0.0f };
    f32 m_rotation = 0.0f;
};

} 
