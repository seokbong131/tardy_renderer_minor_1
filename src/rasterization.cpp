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

    // left (X)
    /*int x = static_cast<int>(std::round((-v.z + 1.0) * 0.5 * width));
    int y = static_cast<int>(std::round((v.y + 1.0) * 0.5 * height));*/

    // top (Y)
    /*int x = static_cast<int>(std::round((v.x + 1.0) * 0.5 * width));
    int y = static_cast<int>(std::round((-v.z + 1.0) * 0.5 * height));*/

    x = std::clamp(x, 0, width - 1);
    y = std::clamp(y, 0, height - 1);

    return { x, y };
}

constexpr TGAColor white    = { 255, 255, 255, 255 };
constexpr TGAColor red      = { 0,     0, 255, 255 };
constexpr TGAColor green    = { 0,   255,   0, 255 };
constexpr TGAColor blue     = { 255,   0,   0, 255 };

// by scanline rendering algorithm
void draw_triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage& framebuffer, TGAColor color) {
    int x_coords[3] = { ax, bx, cx };
    int y_coords[3] = { ay, by, cy };

    // insertion sort by y-coordinate (ascending order)
    for (int i = 1; i < 3; ++i) {
        for (int j = i; j > 0 && y_coords[j] < y_coords[j - 1]; --j) {
            std::swap(x_coords[j], x_coords[j - 1]);
            std::swap(y_coords[j], y_coords[j - 1]);
        }
    }

    int total_height = y_coords[2] - y_coords[0];

    if (y_coords[0] != y_coords[1]) { // lower half
        int lower_height = y_coords[1] - y_coords[0];

        // T := upper half (X) -> include y_coords[1] scanline
        // F := upper half (O) -> avoid overlap
        int y_end = (y_coords[1] == y_coords[2]) ? y_coords[1] : y_coords[1] - 1;

        for (int y = y_coords[0]; y <= y_end; y++) {
            // linear interpolation (integer arithmetic)
            int x_1 = x_coords[0] + ((x_coords[1] - x_coords[0]) * (y - y_coords[0])) / lower_height;
            int x_2 = x_coords[0] + ((x_coords[2] - x_coords[0]) * (y - y_coords[0])) / total_height;

            // boundary
            /*framebuffer.set(x_1, y, blue);
            framebuffer.set(x_2, y, red);*/

            // top-left rule (X) -> overlap
            for (int x = std::min(x_1, x_2); x <= std::max(x_1, x_2); x++)
                framebuffer.set(x, y, (y == y_coords[1]) ? white : color);
        }
    }

    if (y_coords[1] != y_coords[2]) { // upper half
        int upper_height = y_coords[2] - y_coords[1];

        for (int y = y_coords[1]; y <= y_coords[2]; y++) {
            // linear interpolation (integer arithmetic)
            int x_1 = x_coords[1] + ((x_coords[2] - x_coords[1]) * (y - y_coords[1])) / upper_height;
            int x_2 = x_coords[0] + ((x_coords[2] - x_coords[0]) * (y - y_coords[0])) / total_height;

            // boundary
            /*framebuffer.set(x_1, y, green);
            framebuffer.set(x_2, y, red);*/

            // top-left rule (X) -> overlap
            for (int x = std::min(x_1, x_2); x <= std::max(x_1, x_2); x++)
                framebuffer.set(x, y, (y == y_coords[1]) ? white : color);
        }
    }

    /*draw_line(x_coords[0], y_coords[0], x_coords[1], y_coords[1], framebuffer, blue);
    draw_line(x_coords[1], y_coords[1], x_coords[2], y_coords[2], framebuffer, green);
    draw_line(x_coords[2], y_coords[2], x_coords[0], y_coords[0], framebuffer, red);*/
}
