#pragma once

#include "obj_loader.h"
#include "tgaimage.h"

void render_3(const Mesh& mesh,
              int         width,
              int         height,
              TGAColor    color_a,
              TGAColor    color_b,
              TGAColor    color_c,
              TGAImage&   framebuffer);

namespace volume_rendering_effect
{
static void take_triangle_snapshot(int& slice_index, int target, TGAImage& framebuffer);
void        visualize_triangle_slices(int         num_slices,
                                      const Mesh& mesh,
                                      int         width,
                                      int         height,
                                      TGAColor    color_a,
                                      TGAColor    color_b,
                                      TGAColor    color_c,
                                      TGAImage&   framebuffer);

static void take_snapshot(
    int num_slices, int width, int height, const TGAImage& zbuffer, const TGAImage& framebuffer);
void visualize_slices(int         num_slices,
                      const Mesh& mesh,
                      int         width,
                      int         height,
                      TGAColor    color_a,
                      TGAColor    color_b,
                      TGAColor    color_c,
                      TGAImage&   zbuffer,
                      TGAImage&   framebuffer);
}

void render_4(const Mesh& mesh, int width, int height, TGAImage& zbuffer, TGAImage& framebuffer);
