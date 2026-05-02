#pragma once

#include <string>

#include "framebuffer.h"

bool save_color_png(const Framebuffer& framebuffer, const std::string& filename);
bool save_depth_png(const Framebuffer& framebuffer, const std::string& filename);
