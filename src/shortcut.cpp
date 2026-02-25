#include "shortcut.h"

#include <algorithm>

#include "rasterization.h"
#include "utils.h"

// for triangle rasterization
void render_3(const Mesh& mesh,
              int         width,
              int         height,
              TGAColor    color_a,
              TGAColor    color_b,
              TGAColor    color_c,
              TGAImage&   framebuffer) {
    for (int i = 0; i < mesh.num_triangles(); i++) {
        auto [ax, ay] = project_orthographic_2(mesh.get_triangle_vertex(i, 0), width, height);
        auto [bx, by] = project_orthographic_2(mesh.get_triangle_vertex(i, 1), width, height);
        auto [cx, cy] = project_orthographic_2(mesh.get_triangle_vertex(i, 2), width, height);

        interpolate_modern_triangle(ax, ay, color_a, bx, by, color_b, cx, cy, color_c, framebuffer);
    }
}

// backface culling (X)
namespace volume_rendering_effect
{
static void take_triangle_snapshot(int& slice_begin, int slice_end, TGAImage& framebuffer) {
    // half-open ranges [begin, end)
    for (int i = slice_begin; i < slice_end; ++i) {
        framebuffer.write_png_file(set_filename(i));
    }

    slice_begin = slice_end;
}

// in summary,
//     for each triangle:
//         snapshot flush
//         rasterize triangle
void visualize_triangle_slices(int         num_slices,
                               const Mesh& mesh,
                               int         width,
                               int         height,
                               TGAColor    color_a,
                               TGAColor    color_b,
                               TGAColor    color_c,
                               TGAImage&   framebuffer) {
    int    slice_begin   = 0;
    double slice_spacing = (mesh.get_max_depth() - mesh.get_min_depth()) / num_slices;

    // assumption: triangles -> sort by depth (ascending order = rear-to-front order)
    for (int i = 0; i < mesh.num_triangles(); i++) {
        int    slice_end      = 1; // to ensure the first framebuffer
        double triangle_min_z = std::min({mesh.get_triangle_vertex(i, 0).z,
                                          mesh.get_triangle_vertex(i, 1).z,
                                          mesh.get_triangle_vertex(i, 2).z});

        slice_end += std::clamp(
            static_cast<int>((triangle_min_z - mesh.get_min_depth()) / slice_spacing),
            0,
            num_slices - 1);

        take_triangle_snapshot(slice_begin, slice_end, framebuffer);

        auto [ax, ay] = project_orthographic_2(mesh.get_triangle_vertex(i, 0), width, height);
        auto [bx, by] = project_orthographic_2(mesh.get_triangle_vertex(i, 1), width, height);
        auto [cx, cy] = project_orthographic_2(mesh.get_triangle_vertex(i, 2), width, height);

        // solid (random) color
        /*TGAColor random_color;
        for (int elem = 0; elem < 3; elem++)
            random_color[elem] = static_cast<std::uint8_t>(std::rand() % 256);

        draw_modern_triangle(ax, ay, bx, by, cx, cy, framebuffer, random_color);*/

        // gradient color
        interpolate_modern_triangle(ax, ay, color_a, bx, by, color_b, cx, cy, color_c, framebuffer);
    }

    take_triangle_snapshot(slice_begin, num_slices, framebuffer);
}

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

        draw_modern_triangle_with_depth(
            ax, ay, az, bx, by, bz, cx, cy, cz, zbuffer, framebuffer, random_color);

        // gradient color
        // interpolate_modern_triangle(ax, ay, color_a, bx, by, color_b, cx, cy, color_c, framebuffer);
    }

    take_snapshot(num_slices, width, height, zbuffer, framebuffer);
}
}

// for triangle rasterization
void render_4(const Mesh& mesh, int width, int height, TGAImage& zbuffer, TGAImage& framebuffer) {
    for (int i = 0; i < mesh.num_triangles(); i++) {
        auto [ax, ay, az] = project_orthographic_3(mesh.get_triangle_vertex(i, 0), width, height);
        auto [bx, by, bz] = project_orthographic_3(mesh.get_triangle_vertex(i, 1), width, height);
        auto [cx, cy, cz] = project_orthographic_3(mesh.get_triangle_vertex(i, 2), width, height);

        // solid (random) color
        TGAColor random_color;
        for (int elem = 0; elem < 3; elem++)
            random_color[elem] = static_cast<std::uint8_t>(std::rand() % 256);

        draw_modern_triangle_with_depth(
            ax, ay, az, bx, by, bz, cx, cy, cz, zbuffer, framebuffer, random_color);
    }
}
