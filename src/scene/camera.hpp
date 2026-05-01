#pragma once

#include <cmath>

#include "graphics_mathematics/constants.hpp"
#include "graphics_mathematics/matrix_clip_space.hpp"
#include "graphics_mathematics/matrix_transform.hpp"

// ONLY data (default)
struct Camera
{
    // for viewing transformation (dynamic)
    vec3 eye    = {0.0f, 0.0f, 3.0f};
    vec3 center = {0.0f, 0.0f, 0.0f};
    vec3 up     = {0.0f, 1.0f, 0.0f};

    // for projection transformation (static)
    float fov_radian   = 60.0f * TO_RADIAN;
    float aspect_ratio = 1.0f;
    float near         = 0.1f;
    float far          = 100.0f;
};

// WC -> EC
[[nodiscard]] inline mat4 view_matrix(const Camera& camera) {
    return look_at(camera.eye, camera.center, camera.up);
}

// EC -> CC
[[nodiscard]] inline mat4 projection_matrix(const Camera& camera) {
    return project_perspective(camera.fov_radian, camera.aspect_ratio, camera.near, camera.far);
}

// camera movement
inline void set_orbit(
    Camera& camera, const vec3& center, float distance, float height, float theta) {
    camera.center = center;
    camera.eye    = {center.x + distance * std::cos(theta),
                     center.y + height,
                     center.z + distance * std::sin(theta)};
    camera.up     = {0.0f, 1.0f, 0.0f};
}
