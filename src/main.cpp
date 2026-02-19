#include "graphics_mathematics.h"
#include "obj_loader.h"
#include "rasterization.h"
#include "tgaimage.h"
#include "utils.h"

constexpr int width = 1000;
constexpr int height = 1000;

constexpr TGAColor white    = { 255, 255, 255, 255 }; // attention, BGRA order
constexpr TGAColor red      = { 0,     0, 255, 255 };
constexpr TGAColor green    = { 0,   255,   0, 255 };
constexpr TGAColor blue     = { 255,   0,   0, 255 };
constexpr TGAColor cyan     = { 255, 255,   0, 255 };
constexpr TGAColor magenta  = { 255,   0, 255, 255 };
constexpr TGAColor yellow   = { 0,   255, 255, 255 };
constexpr TGAColor gray     = { 127, 127, 127, 255 };
constexpr TGAColor black    = { 0,     0,   0, 255 };

int main(int argc, char** argv) {
    std::string model_path = "../assets/diablo3_pose.obj";
    //std::string model_path_1 = "../assets/african_head/african_head.obj";
    //std::string model_path_2 = "../assets/african_head/african_head_eye_inner.obj";
    //std::string model_path_3 = "../assets/african_head/african_head_eye_outer.obj";
    //std::string model_path_1 = "../assets/boggie/body.obj";
    //std::string model_path_2 = "../assets/boggie/eyes.obj";
    //std::string model_path_3 = "../assets/boggie/head.obj";

    Mesh mesh(model_path);
    //Mesh mesh_1(model_path_1);
    //Mesh mesh_2(model_path_2);
    //Mesh mesh_3(model_path_3);

    TGAImage framebuffer(width, height, TGAImage::RGB);

    // for triangle rasterization
    for (int i = 0; i < mesh.num_triangles(); i++) {
        auto [ax, ay] = project_orthographic(mesh.get_triangle_vertex(i, 0), width, height);
        auto [bx, by] = project_orthographic(mesh.get_triangle_vertex(i, 1), width, height);
        auto [cx, cy] = project_orthographic(mesh.get_triangle_vertex(i, 2), width, height);

        interpolate_modern_triangle(ax, ay, black, bx, by, red, cx, cy, white, framebuffer);
    }

    /*for (int i = 0; i < mesh_1.num_triangles(); i++) {
        auto [ax, ay] = project_orthographic(mesh_1.get_triangle_vertex(i, 0), width, height);
        auto [bx, by] = project_orthographic(mesh_1.get_triangle_vertex(i, 1), width, height);
        auto [cx, cy] = project_orthographic(mesh_1.get_triangle_vertex(i, 2), width, height);

        interpolate_modern_triangle(ax, ay, black, bx, by, red, cx, cy, white, framebuffer);
    }*/

    /*for (int i = 0; i < mesh_2.num_triangles(); i++) {
        auto [ax, ay] = project_orthographic(mesh_2.get_triangle_vertex(i, 0), width, height);
        auto [bx, by] = project_orthographic(mesh_2.get_triangle_vertex(i, 1), width, height);
        auto [cx, cy] = project_orthographic(mesh_2.get_triangle_vertex(i, 2), width, height);

        interpolate_modern_triangle(ax, ay, black, bx, by, green, cx, cy, white, framebuffer);
    }*/

    /*for (int i = 0; i < mesh_3.num_triangles(); i++) {
        auto [ax, ay] = project_orthographic(mesh_3.get_triangle_vertex(i, 0), width, height);
        auto [bx, by] = project_orthographic(mesh_3.get_triangle_vertex(i, 1), width, height);
        auto [cx, cy] = project_orthographic(mesh_3.get_triangle_vertex(i, 2), width, height);

        interpolate_modern_triangle(ax, ay, black, bx, by, blue, cx, cy, white, framebuffer);
    }*/

    //framebuffer.write_tga_file("output/framebuffer.tga");
    framebuffer.write_png_file("output/framebuffer.png");

    return 0;
}
