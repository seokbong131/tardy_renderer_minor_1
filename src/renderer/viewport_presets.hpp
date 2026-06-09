#pragma once

#include "graphics_mathematics/matrix.hpp"
#include "graphics_mathematics/matrix_clip_space.hpp"

// lecture reference
[[nodiscard]] inline mat4 set_default_viewport(int width, int height) {
    return transform_viewport(width / 16, height / 16, width * 7 / 8, height * 7 / 8);
}

[[nodiscard]] inline mat4 set_fullscreen_viewport(int width, int height) {
    return transform_viewport(0, 0, width, height);
}
