#include "terrapch.h"
#include "terra/renderer/orthographic_camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace terra {

OrthographicCamera::OrthographicCamera(f32 left, f32 right, f32 bottom, f32 top) {
    m_projection_matrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
    m_view_matrix = glm::mat4(1.0f);
}

void OrthographicCamera::set_projection(f32 left, f32 right, f32 bottom, f32 top) {
    m_projection_matrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
}

void OrthographicCamera::update_view_matrix() {
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_position) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation), glm::vec3(0, 0, 1));
    
    m_view_matrix = glm::inverse(transform);
}

} 
