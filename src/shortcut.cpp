#include "shortcut.h"

#include "rasterization.h"

// for triangle rasterization
void render(const Mesh& mesh,
            int         width,
            int         height,
            TGAColor    color_a,
            TGAColor    color_b,
            TGAColor    color_c,
            TGAImage&   framebuffer) {
    for (int i = 0; i < mesh.num_triangles(); i++) {
        auto [ax, ay] = project_orthographic(mesh.get_triangle_vertex(i, 0), width, height);
        auto [bx, by] = project_orthographic(mesh.get_triangle_vertex(i, 1), width, height);
        auto [cx, cy] = project_orthographic(mesh.get_triangle_vertex(i, 2), width, height);

        interpolate_modern_triangle(ax, ay, color_a, bx, by, color_b, cx, cy, color_c, framebuffer);
    }
}
