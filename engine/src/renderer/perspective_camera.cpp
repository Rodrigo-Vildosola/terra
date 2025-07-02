#include "terrapch.h"
#include "terra/renderer/perspective_camera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace terra {

PerspectiveCamera::PerspectiveCamera(f32 fov, f32 aspect, f32 near, f32 far)
    : m_fov(fov), m_aspect(aspect), m_near(near), m_far(far)
{
    update_projection_matrix();
    update_view_matrix();
}

void PerspectiveCamera::set_position(const glm::vec3& pos) {
    m_position = pos;
    update_view_matrix();
}

void PerspectiveCamera::set_rotation(f32 pitch, f32 yaw) {
    m_pitch = pitch;
    m_yaw = yaw;
    update_view_matrix();
}

void PerspectiveCamera::update_view_matrix() {
    glm::vec3 direction;
    direction.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    direction.y = sin(glm::radians(m_pitch));
    direction.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

    m_forward = glm::normalize(direction);
    m_view_matrix = glm::lookAt(m_position, m_position + m_forward, m_up);
}

void PerspectiveCamera::update_projection_matrix() {
    m_projection_matrix = glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);

    // WebGPU correction: flip Y (GLM assumes OpenGL clip space)
    m_projection_matrix[1][1] *= -1.0f;
}

void PerspectiveCamera::move(const glm::vec3& local) {
    glm::vec3 forward = get_forward();
    glm::vec3 right = glm::normalize(glm::cross(forward, m_up));
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 world_offset =
        right * local.x +
        up * local.y +
        forward * local.z;

    m_position += world_offset;
    update_view_matrix();
}



} // namespace terra
