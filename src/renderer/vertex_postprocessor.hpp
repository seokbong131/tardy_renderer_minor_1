#pragma once

#include <cmath>

#include "graphics_mathematics/constants.hpp"
#include "graphics_mathematics/matrix.hpp"
#include "graphics_mathematics/vector.hpp"

struct WindowCoord
{
    int   x     = -1;
    int   y     = -1;
    float z     = 0.0f;
    bool  valid = false;
};

// clip-space -> window-space
// Ref. https://wikis.khronos.org/opengl/Vertex_Post-Processing
[[nodiscard]] inline WindowCoord postprocess_vertex(const vec4& clip_coord,
                                                    const mat4& viewport_mat) {
    // skip rather than clip & split
    if (clip_coord.w < EPSILON) return {};

    // perspective divide: clip-space -> NDC space
    float inv_w                   = 1.0f / clip_coord.w;
    vec4  normalized_device_coord = vec4{
        clip_coord.x * inv_w, clip_coord.y * inv_w, clip_coord.z * inv_w, 1.0f}; // homogeneous

    // viewport transform: NDC space -> window-space
    vec4 window_coord = viewport_mat * normalized_device_coord;

    return WindowCoord{.x     = static_cast<int>(std::floor(window_coord.x)),
                       .y     = static_cast<int>(std::floor(window_coord.y)),
                       .z     = window_coord.z,
                       .valid = true};
}
