#pragma once

#include "graphics_mathematics.h"
#include "tgaimage.h"

// implement in the order covered in the lecture

void draw_line(
    int start_x, int start_y, int end_x, int end_y, TGAImage& framebuffer, TGAColor color);
std::tuple<int, int> project_orthographic_2(vec3 v, int width, int height);

void draw_classic_triangle(
    int ax, int ay, int bx, int by, int cx, int cy, TGAImage& framebuffer, TGAColor color);
float compute_signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy);
void  draw_modern_triangle(
     int ax, int ay, int bx, int by, int cx, int cy, TGAImage& framebuffer, TGAColor color);

void draw_temporary_line(
    int start_x, int start_y, int end_x, int end_y, TGAImage& framebuffer, TGAColor color);
void interpolate_modern_triangle(int       ax,
                                 int       ay,
                                 TGAColor  a_color,
                                 int       bx,
                                 int       by,
                                 TGAColor  b_color,
                                 int       cx,
                                 int       cy,
                                 TGAColor  c_color,
                                 TGAImage& framebuffer);

void                      draw_modern_triangle_with_depth(int       ax,
                                                          int       ay,
                                                          int       az,
                                                          int       bx,
                                                          int       by,
                                                          int       bz,
                                                          int       cx,
                                                          int       cy,
                                                          int       cz,
                                                          TGAImage& zbuffer,
                                                          TGAImage& framebuffer,
                                                          TGAColor  color);
std::tuple<int, int, int> project_orthographic_3(vec3 v, int width, int height);
