#pragma once

#include <cmath>

#include "matrix.hpp"

// for orthographic projection (M_p: EC -> CC)
// Ref. https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/orthographic-projection-matrix.html
[[nodiscard]] inline mat4 project_orthographic(
    float left, float right, float bottom, float top, float near, float far) {
    //   C[0]             C[1]             C[2]             C[3]
    // | 2/(R-L)          0                0                (L+R)/(L-R) |
    // | 0                2/(T-B)          0                (B+T)/(B-T) |
    // | 0                0                2/(N-F)          (N+F)/(N-F) |
    // | 0                0                0                1           |
    mat4 projection = {{{2.0f / (right - left), 0.0f, 0.0f, 0.0f},
                        {0.0f, 2.0f / (top - bottom), 0.0f, 0.0f},
                        {0.0f, 0.0f, 2.0f / (near - far), 0.0f},
                        {(left + right) / (left - right),
                         (bottom + top) / (bottom - top),
                         (near + far) / (near - far),
                         1.0f}}};

    return projection;
}

// for perspective projection (M_p: EC -> CC)
// Ref. https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/opengl-perspective-projection-matrix.html
[[nodiscard]] inline mat4 project_perspective(float fov_radian,
                                              float aspect_ratio,
                                              float near,
                                              float far) {
    // focal length
    float f = 1.0f / std::tan(fov_radian / 2.0f);

    //   C[0]             C[1]             C[2]             C[3]
    // | f/a              0                0                0         |
    // | 0                f                0                0         |
    // | 0                0                (F+N)/(N-F)      2FN/(N-F) |
    // | 0                0                -1               0         | (*)
    mat4 projection = {{{f / aspect_ratio, 0.0f, 0.0f, 0.0f},
                        {0.0f, f, 0.0f, 0.0f},
                        {0.0f, 0.0f, (far + near) / (near - far), -1.0f},
                        {0.0f, 0.0f, 2.0f * far * near / (near - far), 0.0f}}};

    return projection;
}

// assumption: x and y specify the lower left corner.
// for viewport transform (M_vp: NDC -> screen space)
// x: [-1, 1] -> [x, x + width - 1]
// y: [-1, 1] -> [y, y + height - 1]
// z: [-1, 1] -> [0, 1] (for depth buffer)
// Ref. https://registry.khronos.org/OpenGL-Refpages/gl4/html/glViewport.xhtml
[[nodiscard]] inline mat4 transform_viewport(int lower_left_corner_x,
                                             int lower_left_corner_y,
                                             int width,
                                             int height) {
    float half_width  = width * 0.5f;
    float half_height = height * 0.5f;

    //   C[0]       C[1]       C[2]       C[3]
    // | w/2        0          0          x + w/2 |
    // | 0          h/2        0          y + h/2 |
    // | 0          0          0.5        0.5     |
    // | 0          0          0          1       |
    mat4 viewport = {
        {{half_width, 0.0f, 0.0f, 0.0f},
         {0.0f, half_height, 0.0f, 0.0f},
         {0.0f, 0.0f, 0.5f, 0.0f},
         {lower_left_corner_x + half_width, lower_left_corner_y + half_height, 0.5f, 1.0f}}};

    return viewport;
}
