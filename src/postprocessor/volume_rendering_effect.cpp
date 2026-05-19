#include "volume_rendering_effect.h"

#include <algorithm>

#include "rasterization.h"
#include "util/system.h"

namespace
{
// assumption: x, y, and z are all in the range [-1, 1]. (WC)
// elevation: (x, y, z) -> (x, y) = 3D -> 2D
// viewport transform
// 1. x and y are all in the range [0, 2].
// 2. x and y are all in the range [0, 1]. (normalization)
// 3. x is in the range [0, width - 1] and y is in the range [0, height - 1]. (screen space)
// 4. z is in the range [0, 255]. (screen space, variable)
std::tuple<int, int, int> project_orthographic_3(vec3 v, int width, int height) {
    // front (Z)
    int x = static_cast<int>(std::round((v.x + 1.0) * 0.5 * width));
    int y = static_cast<int>(std::round((v.y + 1.0) * 0.5 * height));
    int z = static_cast<int>(std::round((v.z + 1.0) * 0.5 * 255.0));

    // left (X)
    /*int x = static_cast<int>(std::round((-v.z + 1.0) * 0.5 * width));
    int y = static_cast<int>(std::round((v.y + 1.0) * 0.5 * height));
    int z = static_cast<int>(std::round((v.x + 1.0) * 0.5 * 255.0));*/

    // top (Y)
    /*int x = static_cast<int>(std::round((v.x + 1.0) * 0.5 * width));
    int y = static_cast<int>(std::round((-v.z + 1.0) * 0.5 * height));
    int z = static_cast<int>(std::round((v.y + 1.0) * 0.5 * 255.0));*/

    x = std::clamp(x, 0, width - 1);
    y = std::clamp(y, 0, height - 1);
    z = std::clamp(z, 0, 255);

    return {x, y, z};
}

#pragma warning(push)
#pragma warning(disable : 6993)
void draw_modern_triangle_with_i_depth(int       ax,
                                       int       ay,
                                       int       az,
                                       int       bx,
                                       int       by,
                                       int       bz,
                                       int       cx,
                                       int       cy,
                                       int       cz,
                                       TGAImage& zbuffer,
                                       TGAImage& framebuffer,
                                       TGAColor  color) {
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
            float beta_area  = compute_signed_triangle_area(ax, ay, x, y, cx, cy) / total_area;
            float gamma_area = compute_signed_triangle_area(ax, ay, bx, by, x, y) / total_area;

            // Area > 0 => pixel is inside the triangle
            // Area = 0 => pixel is on the edge of the triangle
            // Area < 0 => pixel is outside the triangle
            if (alpha_area < 0 || beta_area < 0 || gamma_area < 0) continue;

            // depth interpolation
            uint8_t depth = static_cast<std::uint8_t>(alpha_area * az + beta_area * bz +
                                                      gamma_area * cz);

            // per-pixel painter's algorithm
            // 0 (far) <-> 255 (near => update)
            auto z_value = zbuffer.get(x, y)[0];
            if (depth <= z_value) continue;

            zbuffer.set(x, y, {depth});
            framebuffer.set(x, y, color);
        }
    }
}
#pragma warning(pop)

// case
// (1) accumulation
// (2) per slice
static void take_snapshot(
    int num_slices, int width, int height, const TGAImage& zbuffer, const TGAImage& framebuffer) {
    int    slice_index   = 0;
    double slice_spacing = 255.0 / num_slices;

    TGAImage snapshot(width, height, TGAImage::RGB);
    snapshot.write_png_file(set_filename(slice_index)); // to ensure the first framebuffer

    uint8_t depth_far = 0;

    for (slice_index = 1; slice_index < num_slices; ++slice_index) {
        // for (2), need to rename outside snapshot -> snapshot_0
        // TGAImage snapshot(width, height, TGAImage::RGB);

        uint8_t depth_near = static_cast<uint8_t>(
            std::clamp(static_cast<int>(std::round(slice_index * slice_spacing)), 0, 255));
        /*// for (2)
        uint8_t depth_near_plus_one = static_cast<uint8_t>(
            std::clamp(static_cast<int>(std::round((slice_index + 1) * slice_spacing)), 0, 255));*/

        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                uint8_t z_value = zbuffer.get(x, y)[0];

                // (1) half-open ranges (depth_far, depth_near]
                if (z_value > depth_far && z_value <= depth_near)
                    snapshot.set(x, y, framebuffer.get(x, y));

                /*// (2) half-open ranges (depth_near, depth_near + 1]
                if (z_value > depth_far && z_value > depth_near && z_value <= depth_near_plus_one)
                    snapshot.set(x, y, framebuffer.get(x, y));*/
            }
        }

        snapshot.write_png_file(set_filename(slice_index));

        depth_far = depth_near;
    }
}
}

namespace volume_rendering_effect
{
// in summary,
//     for each slice:
//         render all triangles affecting that slice
//         snapshot
void visualize_slices(int         num_slices,
                      const Mesh& mesh,
                      int         width,
                      int         height,
                      TGAColor    color_a,
                      TGAColor    color_b,
                      TGAColor    color_c,
                      TGAImage&   zbuffer,
                      TGAImage&   framebuffer) {
    for (int i = 0; i < mesh.num_triangles(); i++) {
        auto [ax, ay, az] = project_orthographic_3(mesh.get_triangle_vertex(i, 0), width, height);
        auto [bx, by, bz] = project_orthographic_3(mesh.get_triangle_vertex(i, 1), width, height);
        auto [cx, cy, cz] = project_orthographic_3(mesh.get_triangle_vertex(i, 2), width, height);

        // solid (random) color
        TGAColor random_color;
        for (int elem = 0; elem < 3; elem++)
            random_color[elem] = static_cast<std::uint8_t>(std::rand() % 256);

        draw_modern_triangle_with_i_depth(
            ax, ay, az, bx, by, bz, cx, cy, cz, zbuffer, framebuffer, random_color);

        // gradient color
        // interpolate_modern_triangle(ax, ay, color_a, bx, by, color_b, cx, cy, color_c, framebuffer);
    }

    take_snapshot(num_slices, width, height, zbuffer, framebuffer);
}
}
