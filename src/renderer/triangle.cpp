#include "triangle.h"

#include <algorithm>

// by shoelace formula
float compute_signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy) {
    return 0.5f * ((by - ay) * (bx + ax) + (cy - by) * (cx + bx) + (ay - cy) * (ax + cx));
}

#pragma warning(push)
#pragma warning(disable : 6993)
void draw_triangle(const WindowCoord& window_coord_0,
                   const WindowCoord& window_coord_1,
                   const WindowCoord& window_coord_2,
                   Framebuffer&       framebuffer,
                   Shader&            shader) {
    const int width  = framebuffer.width;
    const int height = framebuffer.height;

    // window-space AABB -> viewport clamping
    int aabb_min_x = std::max(std::min({window_coord_0.x, window_coord_1.x, window_coord_2.x}), 0);
    int aabb_min_y = std::max(std::min({window_coord_0.y, window_coord_1.y, window_coord_2.y}), 0);
    int aabb_max_x = std::min(std::max({window_coord_0.x, window_coord_1.x, window_coord_2.x}),
                              width - 1);
    int aabb_max_y = std::min(std::max({window_coord_0.y, window_coord_1.y, window_coord_2.y}),
                              height - 1);

    float total_area = compute_signed_triangle_area(window_coord_0.x,
                                                    window_coord_0.y,
                                                    window_coord_1.x,
                                                    window_coord_1.y,
                                                    window_coord_2.x,
                                                    window_coord_2.y);
    // total area < 0       => backface culling
    // total area = 0       => avoiding division by zero
    // 0 < total area < 1   => discarding triangle (< a pixel)
    if (total_area < 1) return;

    const float inv_total_area = 1.0f / total_area;

#pragma omp parallel for
    for (int y = aabb_min_y; y <= aabb_max_y; ++y) {
        for (int x = aabb_min_x; x <= aabb_max_x; ++x) {
            // Area(PBC) := alpha, Area(PCA) := beta, Area(PAB) := gamma
            float alpha_area =
                compute_signed_triangle_area(
                    x, y, window_coord_1.x, window_coord_1.y, window_coord_2.x, window_coord_2.y) *
                inv_total_area;
            float beta_area =
                compute_signed_triangle_area(
                    window_coord_0.x, window_coord_0.y, x, y, window_coord_2.x, window_coord_2.y) *
                inv_total_area;
            float gamma_area =
                compute_signed_triangle_area(
                    window_coord_0.x, window_coord_0.y, window_coord_1.x, window_coord_1.y, x, y) *
                inv_total_area;

            // Area > 0 => pixel is inside the triangle
            // Area = 0 => pixel is on the edge of the triangle
            // Area < 0 => pixel is outside the triangle
            if (alpha_area < 0.0f || beta_area < 0.0f || gamma_area < 0.0f) continue;

            // depth interpolation
            float depth = alpha_area * window_coord_0.z + beta_area * window_coord_1.z +
                          gamma_area * window_coord_2.z;

            // depth test
            // the smaller, the nearer (=> update)
            // cast width to size_t to avoid an overflow issue
            const std::size_t idx = static_cast<std::size_t>(width) * y + x;
            if (depth >= framebuffer.depth[idx]) continue;

            // fragment discard
            TGAColor output_color{};
            if (shader.fragment({alpha_area, beta_area, gamma_area}, output_color) == false)
                continue;

            framebuffer.depth[idx] = depth;
            framebuffer.color.set(x, y, output_color);
        }
    }
}
#pragma warning(pop)
