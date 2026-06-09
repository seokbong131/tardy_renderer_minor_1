#pragma once

#include <string_view>

#include "framebuffer/framebuffer.h"

enum class VisualizationType {
    Accumulation,
    Slicing,
};

void visualize_slices(const Framebuffer& framebuffer,
                      int                num_slices,
                      int                field_width,
                      std::string_view   output_folder,
                      std::string_view   slice_buffer,
                      VisualizationType  visualization_type = VisualizationType::Accumulation);
