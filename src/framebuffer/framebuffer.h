#pragma once

#include <limits>
#include <vector>

#include "tgaimage.h"

// the bigger, the farther (background)
inline constexpr float BACKGROUND = std::numeric_limits<float>::max();

struct Framebuffer
{
    int                width  = 0;
    int                height = 0;
    TGAImage           color;
    std::vector<float> depth;
};

[[nodiscard]] Framebuffer allocate(int width, int height);
void                      clear(Framebuffer& framebuffer, TGAColor solid_color = {});
