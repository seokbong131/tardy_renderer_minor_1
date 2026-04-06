#include "utils.h"

#include <format>
#include <iostream>
#include <limits>

#include <omp.h>

#include "config.h"
#include "tgaimage.h"

#pragma warning(push)
#pragma warning(disable : 6993)
void print_openmp_info() {
// check compiler support about _OPENMP macro
#ifdef _OPENMP
    std::cout << "[INFO] OpenMP version: " << _OPENMP << std::endl;
    std::cout << "[INFO] max threads: " << omp_get_max_threads() << std::endl;
    std::cout << "----------------------------------------" << std::endl;

#    pragma omp parallel
    {
#    pragma omp critical
        std::cout << "thread " << omp_get_thread_num() << "\t\t(of " << omp_get_num_threads() << ")"
                  << std::endl;
    }
#else
    std::cout << "OpenMP is not enabled." << std::endl;
#endif
}
#pragma warning(pop)

std::string set_filename(int index) {
    return std::format("{}/{}_{:0{}}.png", OUTPUT_FOLDER, FRAME_BUFFER, index, FIELD_WIDTH);
}

bool zbuffer_write_png_file(const std::string&        filename,
                            const std::vector<float>& depthbuffer,
                            int                       width,
                            int                       height) {
    static constexpr float BACKGROUND = std::numeric_limits<float>::max();

    // for depth range
    float min_z = std::numeric_limits<float>::max();
    float max_z = std::numeric_limits<float>::lowest();

    for (float z : depthbuffer) {
        if (z < BACKGROUND) {
            min_z = std::min(min_z, z);
            max_z = std::max(max_z, z);
        }
    }

    // guard
    if (min_z >= max_z) {
        min_z = 0.0f;
        max_z = 1.0f;
    }

    TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);

    // normalization: depth -> [0, 255] (grayscale)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float z = depthbuffer[static_cast<std::size_t>(width) * y + x];

            std::uint8_t depth = 0;

            if (z < BACKGROUND) {
                float normalized_z = (z - min_z) / (max_z - min_z);
                depth              = static_cast<std::uint8_t>((1.0f - normalized_z) * 255.0f);
            }

            zbuffer.set(x, y, {depth});
        }
    }

    // delegate PNG to TGAImage
    return zbuffer.write_png_file(filename);
}
