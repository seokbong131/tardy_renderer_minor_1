#include "graphics_mathematics.h"
#include "obj_loader.h"
#include "rasterization.h"
#include "tgaimage.h"
#include "utils.h"

constexpr int width = 128;
constexpr int height = 128;

constexpr TGAColor white    = { 255, 255, 255, 255 }; // attention, BGRA order
constexpr TGAColor red      = { 0,     0, 255, 255 };
constexpr TGAColor green    = { 0,   255,   0, 255 };
constexpr TGAColor blue     = { 255,   0,   0, 255 };
constexpr TGAColor cyan     = { 255, 255,   0, 255 };
constexpr TGAColor magenta  = { 255,   0, 255, 255 };
constexpr TGAColor yellow   = { 0,   255, 255, 255 };
constexpr TGAColor black    = { 0,     0,   0, 255 };

int main(int argc, char** argv) {
    TGAImage framebuffer(width, height, TGAImage::RGB);

    /*draw_classic_triangle(7, 45, 35, 100, 45, 60, framebuffer, red);
    draw_classic_triangle(120, 35, 90, 5, 45, 110, framebuffer, green);
    draw_classic_triangle(115, 83, 80, 90, 85, 120, framebuffer, blue);*/

    draw_modern_triangle(7, 45, 35, 100, 45, 60, framebuffer, red);
    draw_modern_triangle(120, 35, 90, 5, 45, 110, framebuffer, green);
    draw_modern_triangle(115, 83, 80, 90, 85, 120, framebuffer, blue);

    //print_openmp_info();

    framebuffer.write_tga_file("output/framebuffer.tga");
    framebuffer.write_png_file("output/framebuffer.png");

    return 0;
}
