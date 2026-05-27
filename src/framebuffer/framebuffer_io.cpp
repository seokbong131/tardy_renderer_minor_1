#include "framebuffer_io.h"

#include <cstdint>

bool save_color_png(const Framebuffer& framebuffer, const std::string& filename) {
    // delegate PNG to TGAImage
    return framebuffer.color.write_png_file(filename);
}

bool save_depth_png(const Framebuffer& framebuffer, const std::string& filename) {
    auto [min_z, max_z] = compute_depth_range(framebuffer);

    // guard
    if (min_z >= max_z) {
        min_z = 0.0f;
        max_z = 1.0f;
    }

    // normalization: depth -> [0, 255] (grayscale)
    TGAImage zbuffer(framebuffer.width, framebuffer.height, TGAImage::GRAYSCALE);

    for (int y = 0; y < framebuffer.height; ++y) {
        for (int x = 0; x < framebuffer.width; ++x) {
            float z = framebuffer.depth[static_cast<std::size_t>(framebuffer.width) * y + x];

            std::uint8_t depth_value = 0;

            if (z < BACKGROUND) {
                float normalized_z = (z - min_z) / (max_z - min_z);
                depth_value        = static_cast<std::uint8_t>((1.0f - normalized_z) * 255.0f);
            }

            zbuffer.set(x, y, {depth_value});
        }
    }

    // delegate PNG to TGAImage
    return zbuffer.write_png_file(filename);
}
