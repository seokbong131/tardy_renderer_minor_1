#include "rasterization.h"

#include <algorithm>

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

// by shoelace formula
float compute_signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy) {
    return 0.5f * ((by - ay) * (bx + ax) + (cy - by) * (cx + bx) + (ay - cy) * (ax + cx));
}

#pragma warning(push)
#pragma warning(disable : 6993)
void draw_modern_triangle_with_f_depth(int                 ax,
                                       int                 ay,
                                       float               az,
                                       int                 bx,
                                       int                 by,
                                       float               bz,
                                       int                 cx,
                                       int                 cy,
                                       float               cz,
                                       int                 width,
                                       int                 height,
                                       std::vector<float>& depthbuffer,
                                       TGAImage&           framebuffer,
                                       TGAColor            color) {
    // screen-space AABB -> viewport clamping
    int aabb_min_x = std::max(std::min({ax, bx, cx}), 0);
    int aabb_min_y = std::max(std::min({ay, by, cy}), 0);
    int aabb_max_x = std::min(std::max({ax, bx, cx}), width - 1);
    int aabb_max_y = std::min(std::max({ay, by, cy}), height - 1);

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
            float beta_area  = compute_signed_triangle_area(ax, ay, x, y, cx, cy) / total_area;
            float gamma_area = compute_signed_triangle_area(ax, ay, bx, by, x, y) / total_area;

            // Area > 0 => pixel is inside the triangle
            // Area = 0 => pixel is on the edge of the triangle
            // Area < 0 => pixel is outside the triangle
            if (alpha_area < 0 || beta_area < 0 || gamma_area < 0) continue;

            // depth interpolation
            float depth = alpha_area * az + beta_area * bz + gamma_area * cz;

            // depth test
            // the smaller, the nearer (=> update)
            // cast width to size_t to avoid an overflow issue
            if (depth >= depthbuffer[static_cast<std::size_t>(width) * y + x]) continue;

            depthbuffer[static_cast<std::size_t>(width) * y + x] = depth;
            framebuffer.set(x, y, color);
        }
    }
}
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 6993)
void interpolate_modern_triangle_with_f_depth(int                 ax,
                                              int                 ay,
                                              float               az,
                                              TGAColor            a_color,
                                              int                 bx,
                                              int                 by,
                                              float               bz,
                                              TGAColor            b_color,
                                              int                 cx,
                                              int                 cy,
                                              float               cz,
                                              TGAColor            c_color,
                                              int                 width,
                                              int                 height,
                                              std::vector<float>& depthbuffer,
                                              TGAImage&           framebuffer) {
    // screen-space AABB -> viewport clamping
    int aabb_min_x = std::max(std::min({ax, bx, cx}), 0);
    int aabb_min_y = std::max(std::min({ay, by, cy}), 0);
    int aabb_max_x = std::min(std::max({ax, bx, cx}), width - 1);
    int aabb_max_y = std::min(std::max({ay, by, cy}), height - 1);

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
            float beta_area  = compute_signed_triangle_area(ax, ay, x, y, cx, cy) / total_area;
            float gamma_area = compute_signed_triangle_area(ax, ay, bx, by, x, y) / total_area;

            // Area > 0 => pixel is inside the triangle
            // Area = 0 => pixel is on the edge of the triangle
            // Area < 0 => pixel is outside the triangle
            if (alpha_area < 0 || beta_area < 0 || gamma_area < 0) continue;

            // depth interpolation
            float depth = alpha_area * az + beta_area * bz + gamma_area * cz;

            // depth test
            // the smaller, the nearer (=> update)
            // cast width to size_t to avoid an overflow issue
            if (depth >= depthbuffer[static_cast<std::size_t>(width) * y + x]) continue;

            depthbuffer[static_cast<std::size_t>(width) * y + x] = depth;

            TGAColor  color{};
            const int bpp = framebuffer.byte_per_pixel();

            for (int elem = 0; elem < bpp; ++elem) {
                color[elem] = static_cast<std::uint8_t>(alpha_area * a_color[elem] +
                                                        beta_area * b_color[elem] +
                                                        gamma_area * c_color[elem]);
            }

            framebuffer.set(x, y, color);
        }
    }
}
#pragma warning(pop)
