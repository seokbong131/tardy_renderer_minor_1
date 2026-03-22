#pragma once

#include <string>
#include <vector>

void print_openmp_info();

std::string set_filename(int index);

bool zbuffer_write_png_file(const std::string&        filename,
                            const std::vector<float>& depthbuffer,
                            int                       width,
                            int                       height);
