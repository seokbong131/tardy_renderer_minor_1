#pragma once

#include <cmath>

#include "graphics_mathematics/matrix_clip_space.hpp"
#include "graphics_mathematics/matrix_transform.hpp"
#include "graphics_mathematics/vector.hpp"

enum class ProjectionType {
    Orthographic,
    Perspective,
};

// ONLY data (default)
struct Camera
{
    // for viewing transformation (dynamic)
    // ----------------------------------------------------------------------------------------------------
    vec3 eye    = {-1.0f, 0.0f, 2.0f};
    vec3 center = {0.0f, 0.0f, 0.0f};
    vec3 up     = {0.0f, 1.0f, 0.0f};
    // ----------------------------------------------------------------------------------------------------

    // for projection transformation (static)
    // ----------------------------------------------------------------------------------------------------
    ProjectionType projection_type = ProjectionType::Perspective;
    // CAUTION: order depends on declaration
    float fov_radian   = 2.0f * std::atan(1.0f / norm(eye - center));
    float aspect_ratio = 1.0f;
    float near         = 0.1f;
    float far          = 100.0f;
    // assumption: x, y, and z are all in the range [-1, 1]. (WC)
    float view_volume_half_height = 1.0f;
    // ----------------------------------------------------------------------------------------------------
};

// WC -> EC
[[nodiscard]] inline mat4 view_matrix(const Camera& camera) {
    return look_at(camera.eye, camera.center, camera.up);
}

// EC -> CC
[[nodiscard]] inline mat4 projection_matrix(const Camera& camera) {
    if (camera.projection_type == ProjectionType::Orthographic) {
        const float view_volume_half_height = camera.view_volume_half_height;
        const float view_volume_half_width  = view_volume_half_height * camera.aspect_ratio;

        return project_orthographic(-view_volume_half_width,
                                    view_volume_half_width,
                                    -view_volume_half_height,
                                    view_volume_half_height,
                                    camera.near,
                                    camera.far);
    }

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
