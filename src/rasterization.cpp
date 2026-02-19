#include <omp.h>
#include "rasterization.h"

constexpr TGAColor white    = { 255, 255, 255, 255 }; // attention, BGRA order
constexpr TGAColor red      = { 0,     0, 255, 255 };
constexpr TGAColor green    = { 0,   255,   0, 255 };
constexpr TGAColor blue     = { 255,   0,   0, 255 };
constexpr TGAColor cyan     = { 255, 255,   0, 255 };
constexpr TGAColor magenta  = { 255,   0, 255, 255 };
constexpr TGAColor yellow   = { 0,   255, 255, 255 };
constexpr TGAColor pink     = { 127, 127, 255, 255 };
constexpr TGAColor mint     = { 127, 255, 127, 255 };
constexpr TGAColor sky_blue = { 255, 127, 127, 255 };

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

// by scanline rendering algorithm
void draw_classic_triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage& framebuffer, TGAColor color) {
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

            // top-left rule (X) -> overlap
            for (int x = std::min(x_1, x_2); x <= std::max(x_1, x_2); x++)
                framebuffer.set(x, y, color);
        }
    }

    if (y_coords[1] != y_coords[2]) { // upper half
        int upper_height = y_coords[2] - y_coords[1];

        for (int y = y_coords[1]; y <= y_coords[2]; y++) {
            // linear interpolation (integer arithmetic)
            int x_1 = x_coords[1] + ((x_coords[2] - x_coords[1]) * (y - y_coords[1])) / upper_height;
            int x_2 = x_coords[0] + ((x_coords[2] - x_coords[0]) * (y - y_coords[0])) / total_height;

            // top-left rule (X) -> overlap
            for (int x = std::min(x_1, x_2); x <= std::max(x_1, x_2); x++)
                framebuffer.set(x, y, color);
        }
    }
}

// by shoelace formula
float compute_signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy) {
    return 0.5f * ((by - ay) * (bx + ax) + (cy - by) * (cx + bx) + (ay - cy) * (ax + cx));
}

#pragma warning(push)
#pragma warning(disable: 6993)
void draw_modern_triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage& framebuffer, TGAColor color) {
    int aabb_min_x = std::min(std::min(ax, bx), cx);
    int aabb_min_y = std::min(std::min(ay, by), cy);
    int aabb_max_x = std::max(std::max(ax, bx), cx);
    int aabb_max_y = std::max(std::max(ay, by), cy);

    float total_area = compute_signed_triangle_area(ax, ay, bx, by, cx, cy);
    // total area < 0       => backface culling
    // total area = 0       => avoiding division by zero
    // 0 < total area < 1   => discarding triangles (< a pixel)
    if (total_area < 1) return;

#pragma omp parallel for
    for (int x = aabb_min_x; x <= aabb_max_x; x++) {
        for (int y = aabb_min_y; y <= aabb_max_y; y++) {
            // Area(PBC) := alpha, Area(PCA) := beta, Area(PAB) := gamma
            float alpha_area = compute_signed_triangle_area(x, y, bx, by, cx, cy) / total_area;
            float beta_area = compute_signed_triangle_area(ax, ay, x, y, cx, cy) / total_area;
            float gamma_area = compute_signed_triangle_area(ax, ay, bx, by, x, y) / total_area;

            // Area > 0 => pixel is inside the triangle
            // Area = 0 => pixel is on the edge of the triangle
            // Area < 0 => pixel is outside the triangle
            if (alpha_area < 0 || beta_area < 0 || gamma_area < 0)
                continue;

            framebuffer.set(x, y, color);
        }
    }
}
#pragma warning(pop)

// by digital differential analyzer (DDA) algorithm <= 2nd attempt, round 1
void draw_temporary_line(int start_x, int start_y, int end_x, int end_y, TGAImage& framebuffer, TGAColor color) {
    for (int x = start_x; x <= end_x; x++) {
        float t = (x - start_x) / static_cast<float>(end_x - start_x);
        int y = static_cast<int>(std::lround(start_y + (end_y - start_y) * t));
        framebuffer.set(x, y, color);
    }
}

#pragma warning(push)
#pragma warning(disable: 6993)
void interpolate_modern_triangle(
    int ax, int ay, TGAColor a_color,
    int bx, int by, TGAColor b_color,
    int cx, int cy, TGAColor c_color,
    TGAImage& framebuffer
) {
    int aabb_min_x = std::min(std::min(ax, bx), cx);
    int aabb_min_y = std::min(std::min(ay, by), cy);
    int aabb_max_x = std::max(std::max(ax, bx), cx);
    int aabb_max_y = std::max(std::max(ay, by), cy);

    float total_area = compute_signed_triangle_area(ax, ay, bx, by, cx, cy);
    // total area < 0       => backface culling
    // total area = 0       => avoiding division by zero
    // 0 < total area < 1   => discarding triangles (< a pixel)
    if (total_area < 1) return;

#pragma omp parallel for
    for (int x = aabb_min_x; x <= aabb_max_x; x++) {
        for (int y = aabb_min_y; y <= aabb_max_y; y++) {
            // Area(PBC) := alpha, Area(PCA) := beta, Area(PAB) := gamma
            float alpha_area = compute_signed_triangle_area(x, y, bx, by, cx, cy) / total_area;
            float beta_area = compute_signed_triangle_area(ax, ay, x, y, cx, cy) / total_area;
            float gamma_area = compute_signed_triangle_area(ax, ay, bx, by, x, y) / total_area;

            // Area > 0 => pixel is inside the triangle
            // Area = 0 => pixel is on the edge of the triangle
            // Area < 0 => pixel is outside the triangle
            if (alpha_area < 0 || beta_area < 0 || gamma_area < 0)
                continue;

            // wireframe effect
            /*if (alpha_area > 0.1f && beta_area > 0.1f && gamma_area > 0.1f)
                continue;*/
            
            // visualization
            //if (alpha_area > 0.1f) framebuffer.set(x, y, magenta);
            //if (beta_area > 0.1f) framebuffer.set(x, y, yellow);
            //if (gamma_area > 0.1f) framebuffer.set(x, y, cyan);

            /*if (alpha_area > 0.1f) {
                if (beta_area > 0.1f) {
                    if (gamma_area > 0.1f)  framebuffer.set(x, y, white);
                    else                    framebuffer.set(x, y, pink);
                }
                else {
                    if (gamma_area > 0.1f)  framebuffer.set(x, y, sky_blue);
                    else                    framebuffer.set(x, y, magenta);
                }
            }
            else {
                if (beta_area > 0.1f) {
                    if (gamma_area > 0.1f)  framebuffer.set(x, y, mint);
                    else                    framebuffer.set(x, y, yellow);
                }
                else {
                    if (gamma_area > 0.1f)  framebuffer.set(x, y, cyan);
                    else                    continue;
                }
            }*/

            TGAColor color{};
            const int bpp = framebuffer.byte_per_pixel();

            for (int elem = 0; elem < bpp; ++elem) {
                color[elem] = static_cast<std::uint8_t>(
                    alpha_area * a_color[elem] +
                    beta_area * b_color[elem] +
                    gamma_area * c_color[elem]
                    );
            }

            framebuffer.set(x, y, color);
        }
    }
}
#pragma warning(pop)
