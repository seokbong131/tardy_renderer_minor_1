#include "config.h"
#include "obj_loader.h"
#include "rasterization.h"
#include "tgaimage.h"

int main(int argc, char** argv) {
    std::string model_path = "../assets/diablo3_pose.obj";
    // std::string model_path_1 = "../assets/african_head/african_head.obj";
    // std::string model_path_2 = "../assets/african_head/african_head_eye_inner.obj";
    // std::string model_path_3 = "../assets/african_head/african_head_eye_outer.obj";
    // std::string model_path_1 = "../assets/boggie/body.obj";
    // std::string model_path_2 = "../assets/boggie/eyes.obj";
    // std::string model_path_3 = "../assets/boggie/head.obj";

    Mesh mesh(model_path);
    // Mesh mesh_1(model_path_1);
    // Mesh mesh_2(model_path_2);
    // Mesh mesh_3(model_path_3);

    TGAImage framebuffer(WIDTH, HEIGHT, TGAImage::RGB);

    // for triangle rasterization
    for (int i = 0; i < mesh.num_triangles(); i++) {
        auto [ax, ay] = project_orthographic(mesh.get_triangle_vertex(i, 0), WIDTH, HEIGHT);
        auto [bx, by] = project_orthographic(mesh.get_triangle_vertex(i, 1), WIDTH, HEIGHT);
        auto [cx, cy] = project_orthographic(mesh.get_triangle_vertex(i, 2), WIDTH, HEIGHT);

        interpolate_modern_triangle(ax, ay, BLACK, bx, by, RED, cx, cy, WHITE, framebuffer);
    }

    /*for (int i = 0; i < mesh_1.num_triangles(); i++) {
        auto [ax, ay] = project_orthographic(mesh_1.get_triangle_vertex(i, 0), WIDTH, HEIGHT);
        auto [bx, by] = project_orthographic(mesh_1.get_triangle_vertex(i, 1), WIDTH, HEIGHT);
        auto [cx, cy] = project_orthographic(mesh_1.get_triangle_vertex(i, 2), WIDTH, HEIGHT);

        interpolate_modern_triangle(ax, ay, BLACK, bx, by, RED, cx, cy, WHITE, framebuffer);
    }*/

    /*for (int i = 0; i < mesh_2.num_triangles(); i++) {
        auto [ax, ay] = project_orthographic(mesh_2.get_triangle_vertex(i, 0), WIDTH, HEIGHT);
        auto [bx, by] = project_orthographic(mesh_2.get_triangle_vertex(i, 1), WIDTH, HEIGHT);
        auto [cx, cy] = project_orthographic(mesh_2.get_triangle_vertex(i, 2), WIDTH, HEIGHT);

        interpolate_modern_triangle(ax, ay, BLACK, bx, by, GREEN, cx, cy, WHITE, framebuffer);
    }*/

    /*for (int i = 0; i < mesh_3.num_triangles(); i++) {
        auto [ax, ay] = project_orthographic(mesh_3.get_triangle_vertex(i, 0), WIDTH, HEIGHT);
        auto [bx, by] = project_orthographic(mesh_3.get_triangle_vertex(i, 1), WIDTH, HEIGHT);
        auto [cx, cy] = project_orthographic(mesh_3.get_triangle_vertex(i, 2), WIDTH, HEIGHT);

        interpolate_modern_triangle(ax, ay, BLACK, bx, by, BLUE, cx, cy, WHITE, framebuffer);
    }*/

    // framebuffer.write_tga_file("output/framebuffer.tga");
    framebuffer.write_png_file("output/framebuffer.png");

    return 0;
}
