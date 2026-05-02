#include "framebuffer.h"

#include <algorithm>

Framebuffer allocate(int width, int height) {
    return Framebuffer{
        .width  = width,
        .height = height,
        .color  = TGAImage(width, height, TGAImage::RGB),
        .depth  = std::vector<float>(static_cast<std::size_t>(width) * height, BACKGROUND)};
}

void clear(Framebuffer& framebuffer, TGAColor solid_color) {
    // framebuffer.color
    for (int y = 0; y < framebuffer.height; ++y) {
        for (int x = 0; x < framebuffer.width; ++x)
            framebuffer.color.set(x, y, solid_color);
    }

    // framebuffer.depth
    std::fill(framebuffer.depth.begin(), framebuffer.depth.end(), BACKGROUND);
}
