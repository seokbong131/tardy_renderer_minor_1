#pragma once

#include "tgaimage.h"

float compute_signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy);

void draw_modern_triangle_with_f_depth(int                 ax,
                                       int                 ay,
                                       float               az,
                                       int                 bx,
                                       int                 by,
                                       float               bz,
                                       int                 cx,
                                       int                 cy,
                                       float               cz,
                                       int                 width,
                                       int                 height,
                                       std::vector<float>& depthbuffer,
                                       TGAImage&           framebuffer,
                                       TGAColor            color);

void interpolate_modern_triangle_with_f_depth(int                 ax,
                                              int                 ay,
                                              float               az,
                                              TGAColor            a_color,
                                              int                 bx,
                                              int                 by,
                                              float               bz,
                                              TGAColor            b_color,
                                              int                 cx,
                                              int                 cy,
                                              float               cz,
                                              TGAColor            c_color,
                                              int                 width,
                                              int                 height,
                                              std::vector<float>& depthbuffer,
                                              TGAImage&           framebuffer);
