#pragma once

#include "framebuffer/tgaimage.h"

void draw_line(
    int start_x, int start_y, int end_x, int end_y, TGAImage& framebuffer, TGAColor color);
