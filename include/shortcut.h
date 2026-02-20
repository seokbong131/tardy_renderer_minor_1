#pragma once

#include "obj_loader.h"
#include "tgaimage.h"

// for triangle rasterization
void render(const Mesh& mesh,
            int         width,
            int         height,
            TGAColor    color_a,
            TGAColor    color_b,
            TGAColor    color_c,
            TGAImage&   framebuffer);
