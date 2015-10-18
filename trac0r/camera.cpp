#include "camera.hpp"

#include <glm/glm.hpp>

Camera::Camera() {
}

Camera::Camera(glm::vec3 pos, glm::vec3 dir, glm::vec3 up, float vertical_fov_degrees,
               float near_plane_dist, float far_plane_dist, int screen_width, int screen_height)
    : m_pos(pos), m_dir(dir), m_up(up), m_near_plane_dist(near_plane_dist),
      m_far_plane_dist(far_plane_dist), m_screen_width(screen_width),
      m_screen_height(screen_height) {

    set_vertical_fov(vertical_fov_degrees);
}

glm::vec3 Camera::pos() const {
    return m_pos;
}

void Camera::set_pos(glm::vec3 pos) {
    m_pos = pos;
}

glm::vec3 Camera::dir() const {
    return m_dir;
}

void Camera::set_dir(glm::vec3 dir) {
    m_dir = dir;
}

glm::vec3 Camera::up() const {
    return m_up;
}

void Camera::set_up(glm::vec3 up) {
    m_up = up;
}

float Camera::near_plane_dist() const {
    return m_near_plane_dist;
}

void Camera::set_near_plane_dist(float dist) {
    m_near_plane_dist = dist;
}

float Camera::far_plane_dist() const {
    return m_far_plane_dist;
}

void Camera::set_far_plane_dist(float dist) {
    m_far_plane_dist = dist;
}

float Camera::vertical_fov() const {
    return m_vertical_fov;
}

void Camera::set_vertical_fov(float degrees) {
    // See https://en.wikipedia.org/wiki/Field_of_view_in_video_games
    m_vertical_fov = glm::radians(degrees);
    m_horizontal_fov = 2 * glm::atan(glm::tan(m_vertical_fov / 2) *
                                     ((float)m_screen_width / (float)m_screen_height));
}

float Camera::horizontal_fov() const {
    return m_horizontal_fov;
}

float Camera::aspect_ratio() const {
    return m_screen_width / m_screen_height;
}

glm::vec2 Camera::screenspace_to_camspace(int x, int y) const {
    auto rel_x = (x - m_screen_width / 2.f) / m_screen_width;
    auto rel_y = (y - m_screen_height / 2.f) / m_screen_height;
    return {rel_x, rel_y};
}

glm::vec3 Camera::camspace_to_worldspace(glm::vec2 rel_pos, glm::vec3 canvas_center_pos,
                                         glm::vec3 canvas_dir_x, glm::vec3 canvas_dir_y) const {
    auto canvas_pos = canvas_center_pos + (rel_pos.x * canvas_dir_x) + (rel_pos.y * canvas_dir_y);
    return canvas_pos;
}