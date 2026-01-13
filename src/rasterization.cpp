#include "rasterization.h"

// by Bresenham's line drawing algorithm
void draw_line(int start_x, int start_y, int end_x, int end_y, TGAImage& framebuffer, TGAColor color) {
    bool steep = std::abs(start_x - end_x) < std::abs(start_y - end_y);
    if (steep) { // transpose
        std::swap(start_x, start_y);
        std::swap(end_x, end_y);
    }
    if (start_x > end_x) { // right -> left
        std::swap(start_x, end_x);
        std::swap(start_y, end_y);
    }
    int y = start_y;
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

// assumption: x, y, and z are all in the range [-1, 1]. (WC)
// elevation: (x, y, z) -> (x, y) = 3D -> 2D
// viewport transform
// 1. x and y are all in the range [0, 2].
// 2. x and y are all in the range [0, 1]. (normalization)
// 3. x is in the range [0, width - 1] and y is in the range [0, height - 1]. (screen space)
std::tuple<int, int> project_orthographic(vec3 v, int width, int height) {
    // front (Z)
    int x = static_cast<int>(std::round((v.x + 1.0) * 0.5 * width));
    int y = static_cast<int>(std::round((v.y + 1.0) * 0.5 * height));

    /*// left (X)
    int x = static_cast<int>(std::round((-v.z + 1.0) * 0.5 * width));
    int y = static_cast<int>(std::round((v.y + 1.0) * 0.5 * height));*/

    /*// top (Y)
    int x = static_cast<int>(std::round((v.x + 1.0) * 0.5 * width));
    int y = static_cast<int>(std::round((-v.z + 1.0) * 0.5 * height));*/

    x = std::clamp(x, 0, width - 1);
    y = std::clamp(y, 0, height - 1);

    return { x, y };
}
