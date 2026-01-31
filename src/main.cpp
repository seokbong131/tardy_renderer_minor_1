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
constexpr TGAColor black    = { 0,     0,   0, 255 };

int main(int argc, char** argv) {
    std::string model_path = "../assets/diablo3_pose.obj";

    if (argc == 2) {
        model_path = argv[1];
    }

    Mesh mesh(model_path);

    TGAImage framebuffer(width, height, TGAImage::RGB);

    // for triangle rasterization
    for (int i = 0; i < mesh.num_triangles(); i++) {
        auto [ax, ay] = project_orthographic(mesh.get_triangle_vertex(i, 0), width, height);
        auto [bx, by] = project_orthographic(mesh.get_triangle_vertex(i, 1), width, height);
        auto [cx, cy] = project_orthographic(mesh.get_triangle_vertex(i, 2), width, height);

        TGAColor random_color;
        for (int elem = 0; elem < 3; elem++)
            random_color[elem] = static_cast<std::uint8_t>(std::rand() % 256);

        draw_modern_triangle(ax, ay, bx, by, cx, cy, framebuffer, random_color);
    }

    //print_openmp_info();

    framebuffer.write_tga_file("output/framebuffer.tga");
    framebuffer.write_png_file("output/framebuffer.png");

    return 0;
}
