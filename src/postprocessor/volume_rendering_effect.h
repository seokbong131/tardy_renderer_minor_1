#pragma once

#include "framebuffer/tgaimage.h"
#include "scene/obj_loader.h"

namespace volume_rendering_effect
{
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
