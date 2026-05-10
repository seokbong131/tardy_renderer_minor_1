#include "line.h"

#include <cmath>
#include <utility>

// by Bresenham's line drawing algorithm
void draw_line(
    int start_x, int start_y, int end_x, int end_y, TGAImage& framebuffer, TGAColor color) {
    bool steep = std::abs(start_x - end_x) < std::abs(start_y - end_y);

    if (steep) { // transpose
        std::swap(start_x, start_y);
        std::swap(end_x, end_y);
    }

    if (start_x > end_x) { // right -> left
        std::swap(start_x, end_x);
        std::swap(start_y, end_y);
    }

    int y      = start_y;
    int ierror = 0;

    for (int x = start_x; x <= end_x; x++) {
        if (steep) // de-transpose
            framebuffer.set(y, x, color);
        else
            framebuffer.set(x, y, color);

        ierror += 2 * std::abs(end_y - start_y);

        if (ierror > end_x - start_x) {
            y += end_y > start_y ? 1 : -1;
            ierror -= 2 * (end_x - start_x);
        }
    }
}
