#pragma once

#include "terrapch.h"
#include "terra/renderer/camera.h"

namespace terra {

class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera(f32 fov, f32 aspect, f32 near, f32 far);

    void set_position(const glm::vec3& pos);
    void set_rotation(f32 pitch, f32 yaw);
    void update_view_matrix();
    void update_projection_matrix();

    const glm::vec3& get_position() const { return m_position; }
    glm::vec2 get_rotation() const { return { m_pitch, m_yaw }; }
    const glm::vec3& get_forward() const { return m_forward; }

    f32 get_fov() const { return m_fov; }
    void set_fov(f32 fov) {
        m_fov = fov;
        update_projection_matrix();
    }

    // new projection setters/getters
    void set_near_plane(f32 n)   { m_near = n; update_projection_matrix(); }
    void set_far_plane (f32 f)   { m_far  = f; update_projection_matrix(); }
    void set_aspect_ratio(f32 a) { m_aspect = a; update_projection_matrix(); }

    f32 get_near_plane()  const { return m_near; }
    f32 get_far_plane()   const { return m_far; }
    f32 get_aspect_ratio()const { return m_aspect; }

    void move(const glm::vec3& local); 

private:
    glm::vec3 m_position = { 0.0f, 0.0f, 3.0f };
    glm::vec3 m_forward = { 0.0f, 0.0f, -1.0f };
    glm::vec3 m_up = { 0.0f, 1.0f, 0.0f };

    f32 m_pitch = 0.0f;
    f32 m_yaw = -90.0f; // looking along -Z
    f32 m_fov;
    f32 m_aspect;
    f32 m_near;
    f32 m_far;
};

} 
