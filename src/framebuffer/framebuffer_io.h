#pragma once

#include <string_view>

#include "framebuffer.h"

bool save_color_png(const Framebuffer& framebuffer,
                    std::string_view   output_folder,
                    std::string_view   color_buffer);
bool save_depth_png(const Framebuffer& framebuffer,
                    std::string_view   output_folder,
                    std::string_view   depth_buffer);
