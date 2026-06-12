#include "framebuffer.h"

#include <algorithm>
#include <limits>

Framebuffer allocate(int width, int height) {
    return Framebuffer{
        .width  = width,
        .height = height,
        .color  = TGAImage(width, height, TGAImage::RGB),
        .depth  = std::vector<float>(static_cast<std::size_t>(width) * height, BACKGROUND)};
}

void clear(Framebuffer& framebuffer, TGAColor solid_color) {
    const int width  = framebuffer.width;
    const int height = framebuffer.height;

    // framebuffer.color
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x)
            framebuffer.color.set(x, y, solid_color);
    }

    // framebuffer.depth
    std::fill(framebuffer.depth.begin(), framebuffer.depth.end(), BACKGROUND);
}

std::pair<float, float> compute_depth_range(const Framebuffer& framebuffer) {
    float min_z = std::numeric_limits<float>::max();
    float max_z = std::numeric_limits<float>::lowest();

    for (float z : framebuffer.depth) {
        if (z < BACKGROUND) {
            min_z = std::min(min_z, z);
            max_z = std::max(max_z, z);
        }
    }

    return {min_z, max_z};
}
