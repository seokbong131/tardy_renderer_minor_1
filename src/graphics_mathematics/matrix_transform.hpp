#pragma once

#include "matrix.hpp"

// after this transformation,
//     camera position: the origin
//     camera direction: the negative Z-axis
// for viewing transformation (M_v: WC -> EC)
// 1. translate: eye -> the origin
// 2. rotate: the camera frame -> eye space
// Ref. https://wikis.khronos.org/opengl/Viewing_and_Transformations
[[nodiscard]] inline mat4 look_at(const vec3& eye, const vec3& center, const vec3& rough_up) {
    // the camera frame (WC)
    vec3 backward = normalized(eye - center);              // the positive Z-axis
    vec3 right    = normalized(cross(rough_up, backward)); // the positive X-axis
    vec3 up       = cross(backward, right);                // the positive Y-axis

    //   C[0]       C[1]       C[2]       C[3]
    // | R.x        R.y        R.z        0    |
    // | U.x        U.y        U.z        0    |
    // | B.x        B.y        B.z        0    |
    // | 0          0          0          1    |
    mat4 rotation = {{{right.x, up.x, backward.x, 0.0f},
                      {right.y, up.y, backward.y, 0.0f},
                      {right.z, up.z, backward.z, 0.0f},
                      {0.0f, 0.0f, 0.0f, 1.0f}}};

    //   C[0]   C[1]   C[2]   C[3]
    // | 1      0      0      -E.x |
    // | 0      1      0      -E.y |
    // | 0      0      1      -E.z |
    // | 0      0      0      1    |
    mat4 translation = {{{1.0f, 0.0f, 0.0f, 0.0f},
                         {0.0f, 1.0f, 0.0f, 0.0f},
                         {0.0f, 0.0f, 1.0f, 0.0f},
                         {-eye.x, -eye.y, -eye.z, 1.0f}}};

    return rotation * translation;
}
