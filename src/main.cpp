#include "graphics_mathematics.h"
#include "obj_loader.h"
#include "rasterization.h"
#include "tgaimage.h"

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
    //std::string model_path_1 = "../assets/african_head/african_head.obj";
    //std::string model_path_2 = "../assets/african_head/african_head_eye_inner.obj";
    //std::string model_path_3 = "../assets/african_head/african_head_eye_outer.obj";
    //std::string model_path_1 = "../assets/boggie/body.obj";
    //std::string model_path_2 = "../assets/boggie/eyes.obj";
    //std::string model_path_3 = "../assets/boggie/head.obj";

    if (argc == 2) {
        model_path = argv[1];
        //model_path_1 = argv[1];
    }
    //if (argc == 3) model_path_2 = argv[2];
    //if (argc == 4) model_path_3 = argv[3];

    Mesh mesh(model_path);
    //Mesh mesh_1(model_path_1);
    //Mesh mesh_2(model_path_2);
    //Mesh mesh_3(model_path_3);

    TGAImage framebuffer(width, height, TGAImage::RGB);

    // for line drawing
    /*int ax = 7, ay = 3;
    int bx = 12, by = 37;
    int cx = 62, cy = 53;

    draw_line(bx, by, ax, ay, framebuffer, cyan);
    draw_line(ax, ay, bx, by, framebuffer, red);
    draw_line(cx, cy, bx, by, framebuffer, magenta);
    draw_line(bx, by, cx, cy, framebuffer, green);
    draw_line(cx, cy, ax, ay, framebuffer, yellow);
    draw_line(ax, ay, cx, cy, framebuffer, blue);

    framebuffer.set(ax, ay, white);
    framebuffer.set(bx, by, white);
    framebuffer.set(cx, cy, white);*/

    // for performance measurement about line drawing
    /*std::srand(std::time({}));
    for (int i = 0; i < (1 << 24); i++) {
        int ax = rand() % width, ay = rand() % height;
        int bx = rand() % width, by = rand() % height;
        draw_line(ax, ay, bx, by, framebuffer,
            { static_cast<std::uint8_t>(rand() % 256),
              static_cast<std::uint8_t>(rand() % 256),
              static_cast<std::uint8_t>(rand() % 256),
              static_cast<std::uint8_t>(rand() % 256)
            });
    }*/

    // for wireframe rendering
    for (int i = 0; i < mesh.num_triangles(); i++) {
        auto [ax, ay] = project_orthographic(mesh.get_triangle_vertex(i, 0), width, height);
        auto [bx, by] = project_orthographic(mesh.get_triangle_vertex(i, 1), width, height);
        auto [cx, cy] = project_orthographic(mesh.get_triangle_vertex(i, 2), width, height);

        draw_line(ax, ay, bx, by, framebuffer, red);
        draw_line(bx, by, cx, cy, framebuffer, red);
        draw_line(cx, cy, ax, ay, framebuffer, red);
    }

    for (int i = 0; i < mesh.num_vertices(); i++) {
        vec3 v = mesh.get_vertex(i);
        auto [x, y] = project_orthographic(v, width, height);

        framebuffer.set(x, y, yellow);
    }

    /*for (int i = 0; i < mesh_1.num_triangles(); i++) {
        auto [ax, ay] = project_orthographic(mesh_1.get_triangle_vertex(i, 0), width, height);
        auto [bx, by] = project_orthographic(mesh_1.get_triangle_vertex(i, 1), width, height);
        auto [cx, cy] = project_orthographic(mesh_1.get_triangle_vertex(i, 2), width, height);

        draw_line(ax, ay, bx, by, framebuffer, white);
        draw_line(bx, by, cx, cy, framebuffer, white);
        draw_line(cx, cy, ax, ay, framebuffer, white);
    }

    for (int i = 0; i < mesh_1.num_vertices(); i++) {
        vec3 v = mesh_1.get_vertex(i);
        auto [x, y] = project_orthographic(v, width, height);

        framebuffer.set(x, y, green);
    }*/

    /*for (int i = 0; i < mesh_2.num_triangles(); i++) {
        auto [ax, ay] = project_orthographic(mesh_2.get_triangle_vertex(i, 0), width, height);
        auto [bx, by] = project_orthographic(mesh_2.get_triangle_vertex(i, 1), width, height);
        auto [cx, cy] = project_orthographic(mesh_2.get_triangle_vertex(i, 2), width, height);

        draw_line(ax, ay, bx, by, framebuffer, white);
        draw_line(bx, by, cx, cy, framebuffer, white);
        draw_line(cx, cy, ax, ay, framebuffer, white);
    }

    for (int i = 0; i < mesh_2.num_vertices(); i++) {
        vec3 v = mesh_2.get_vertex(i);
        auto [x, y] = project_orthographic(v, width, height);

        framebuffer.set(x, y, green);
    }*/

    /*for (int i = 0; i < mesh_3.num_triangles(); i++) {
        auto [ax, ay] = project_orthographic(mesh_3.get_triangle_vertex(i, 0), width, height);
        auto [bx, by] = project_orthographic(mesh_3.get_triangle_vertex(i, 1), width, height);
        auto [cx, cy] = project_orthographic(mesh_3.get_triangle_vertex(i, 2), width, height);

        draw_line(ax, ay, bx, by, framebuffer, white);
        draw_line(bx, by, cx, cy, framebuffer, white);
        draw_line(cx, cy, ax, ay, framebuffer, white);
    }

    for (int i = 0; i < mesh_3.num_vertices(); i++) {
        vec3 v = mesh_3.get_vertex(i);
        auto [x, y] = project_orthographic(v, width, height);

        framebuffer.set(x, y, green);
    }*/

    framebuffer.write_tga_file("output/framebuffer.tga");
    framebuffer.write_png_file("output/framebuffer.png");

    return 0;
}
