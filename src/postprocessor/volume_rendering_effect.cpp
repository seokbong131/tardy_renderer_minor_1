#include "volume_rendering_effect.h"

#include <cmath>
#include <filesystem>
#include <format>
#include <iostream>
#include <string>

#include "framebuffer/tgaimage.h"

static std::string set_filename(int              slice_index,
                                int              field_width,
                                std::string_view output_folder,
                                std::string_view slice_buffer) {
    std::filesystem::path filename = std::format(
        "{}_{:0{}}.png", slice_buffer, slice_index, field_width);

    // automatically handle directory-separator
    // Ref. https://en.cppreference.com/cpp/filesystem/path
    return (std::filesystem::path(output_folder) / filename).string();
}

#pragma warning(push)
#pragma warning(disable : 6993)
void visualize_slices(const Framebuffer& framebuffer,
                      int                num_slices,
                      int                field_width,
                      std::string_view   output_folder,
                      std::string_view   slice_buffer,
                      VisualizationType  visualization_type) {
    if (num_slices <= 0) {
        std::cerr << "[ERROR] The number of slices must be greater than 0. (NUM_SLICES: "
                  << num_slices << ")" << std::endl;

        return;
    }

    auto [min_z, max_z] = compute_depth_range(framebuffer);

    // guard
    if (min_z >= max_z) {
        std::cerr << "[WARNING] There is no valid depth range to slice. (min_z: " << min_z
                  << ", max_z: " << max_z << ")" << std::endl;

        return;
    }

    const float slice_spacing = (max_z - min_z) / num_slices;

    const int width  = framebuffer.width;
    const int height = framebuffer.height;
    TGAImage  slice(width, height, TGAImage::RGB);

    float depth_far = std::nextafter(max_z, BACKGROUND);

    for (int slice_index = 0; slice_index < num_slices; ++slice_index) {
        if (visualization_type == VisualizationType::Slicing) {
            slice = TGAImage(width, height, TGAImage::RGB); // reset
        }

        // T: the last slice / F: each spacing
        float depth_near = (slice_index == num_slices - 1)
                               ? min_z
                               : max_z - slice_spacing * (slice_index + 1);

#pragma omp parallel for
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const std::size_t idx = static_cast<std::size_t>(width) * y + x;
                float             z   = framebuffer.depth[idx];

                // half-open range: [depth_near, depth_far)
                if (z >= depth_near && z < depth_far) {
                    slice.set(x, y, framebuffer.color.get(x, y));
                }
            }
        }

        // save slice png
        slice.write_png_file(set_filename(slice_index, field_width, output_folder, slice_buffer));

        int progress = static_cast<int>((slice_index + 1) * 100.0f / num_slices);
        std::cout << std::format("\r[INFO] slicing... ({}%)", progress) << std::flush;

        // initialization for next spacing
        depth_far = depth_near;
    }

    std::cout << std::endl;
}
#pragma warning(pop)
