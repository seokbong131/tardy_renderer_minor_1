#include "framebuffer_io.h"

#include <cstdint>
#include <format>
#include <string>

bool save_color_png(const Framebuffer& framebuffer,
                    std::string_view   output_folder,
                    std::string_view   color_buffer) {
    const std::string filename = std::format("{}/{}.png", output_folder, color_buffer);

    // delegate PNG to TGAImage
    return framebuffer.color.write_png_file(filename);
}

bool save_depth_png(const Framebuffer& framebuffer,
                    std::string_view   output_folder,
                    std::string_view   depth_buffer) {
    auto [min_z, max_z] = compute_depth_range(framebuffer);

    // guard
    if (min_z >= max_z) {
        min_z = 0.0f;
        max_z = 1.0f;
    }

    // normalization: depth -> [0, 255] (grayscale)
    const int width  = framebuffer.width;
    const int height = framebuffer.height;
    TGAImage  zbuffer(width, height, TGAImage::GRAYSCALE);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const std::size_t idx = static_cast<std::size_t>(framebuffer.width) * y + x;
            float             z   = framebuffer.depth[idx];

            std::uint8_t depth_value = 0;

            if (z < BACKGROUND) {
                float normalized_z = (z - min_z) / (max_z - min_z);
                depth_value        = static_cast<std::uint8_t>((1.0f - normalized_z) * 255.0f);
            }

            zbuffer.set(x, y, {depth_value});
        }
    }

    const std::string filename = std::format("{}/{}.png", output_folder, depth_buffer);

    // delegate PNG to TGAImage
    return zbuffer.write_png_file(filename);
}
