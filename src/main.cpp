#include "graphics.h"
#include "obj_loader.h"
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

// by Bresenham's line drawing algorithm
static void draw_line(int start_x, int start_y, int end_x, int end_y, TGAImage& framebuffer, TGAColor color) {
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
static std::tuple<int, int> project_orthographic(vec3 v) {
    // front (Z)
    int x = static_cast<int>(std::round((v.x + 1.0) * 0.5 * width));
    int y = static_cast<int>(std::round((v.y + 1.0) * 0.5 * height));
    
    /*// left (X)
    int x = static_cast<int>(std::round((-v.z + 1.0) * 0.5 * width));
    int y = static_cast<int>(std::round((v.y + 1.0) * 0.5 * height));*/
    
    /*// top (Y)
    int x = static_cast<int>(std::round((v.x + 1.0) * 0.5 * width));
    int y = static_cast<int>(std::round((-v.z + 1.0) * 0.5 * height));*/

    x = std::clamp(x, 0, width - 1);
    y = std::clamp(y, 0, height - 1);

    return { x, y };
}

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
        auto [ax, ay] = project_orthographic(mesh.get_triangle_vertex(i, 0));
        auto [bx, by] = project_orthographic(mesh.get_triangle_vertex(i, 1));
        auto [cx, cy] = project_orthographic(mesh.get_triangle_vertex(i, 2));

        draw_line(ax, ay, bx, by, framebuffer, red);
        draw_line(bx, by, cx, cy, framebuffer, red);
        draw_line(cx, cy, ax, ay, framebuffer, red);
    }

    for (int i = 0; i < mesh.num_vertices(); i++) {
        vec3 v = mesh.get_vertex(i);
        auto [x, y] = project_orthographic(v);

        framebuffer.set(x, y, yellow);
    }

    /*for (int i = 0; i < mesh_1.num_triangles(); i++) {
        auto [ax, ay] = project_orthographic(mesh_1.get_triangle_vertex(i, 0));
        auto [bx, by] = project_orthographic(mesh_1.get_triangle_vertex(i, 1));
        auto [cx, cy] = project_orthographic(mesh_1.get_triangle_vertex(i, 2));

        draw_line(ax, ay, bx, by, framebuffer, white);
        draw_line(bx, by, cx, cy, framebuffer, white);
        draw_line(cx, cy, ax, ay, framebuffer, white);
    }

    for (int i = 0; i < mesh_1.num_vertices(); i++) {
        vec3 v = mesh_1.get_vertex(i);
        auto [x, y] = project_orthographic(v);

        framebuffer.set(x, y, green);
    }*/

    /*for (int i = 0; i < mesh_2.num_triangles(); i++) {
        auto [ax, ay] = project_orthographic(mesh_2.get_triangle_vertex(i, 0));
        auto [bx, by] = project_orthographic(mesh_2.get_triangle_vertex(i, 1));
        auto [cx, cy] = project_orthographic(mesh_2.get_triangle_vertex(i, 2));

        draw_line(ax, ay, bx, by, framebuffer, white);
        draw_line(bx, by, cx, cy, framebuffer, white);
        draw_line(cx, cy, ax, ay, framebuffer, white);
    }

    for (int i = 0; i < mesh_2.num_vertices(); i++) {
        vec3 v = mesh_2.get_vertex(i);
        auto [x, y] = project_orthographic(v);

        framebuffer.set(x, y, green);
    }*/

    /*for (int i = 0; i < mesh_3.num_triangles(); i++) {
        auto [ax, ay] = project_orthographic(mesh_3.get_triangle_vertex(i, 0));
        auto [bx, by] = project_orthographic(mesh_3.get_triangle_vertex(i, 1));
        auto [cx, cy] = project_orthographic(mesh_3.get_triangle_vertex(i, 2));

        draw_line(ax, ay, bx, by, framebuffer, white);
        draw_line(bx, by, cx, cy, framebuffer, white);
        draw_line(cx, cy, ax, ay, framebuffer, white);
    }

    for (int i = 0; i < mesh_3.num_vertices(); i++) {
        vec3 v = mesh_3.get_vertex(i);
        auto [x, y] = project_orthographic(v);

        framebuffer.set(x, y, green);
    }*/

    framebuffer.write_tga_file("output/framebuffer.tga");
    framebuffer.write_png_file("output/framebuffer.png");
    return 0;
}
