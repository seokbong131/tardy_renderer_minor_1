#pragma once

#include "framebuffer/framebuffer.h"
#include "shader/shader.hpp"
#include "vertex_postprocessor.hpp"

[[nodiscard]] float compute_signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy);

void draw_triangle(const WindowCoord& window_coord_0,
                   const WindowCoord& window_coord_1,
                   const WindowCoord& window_coord_2,
                   Framebuffer&       framebuffer,
                   Shader&            shader);
