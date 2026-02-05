#include "graphics_mathematics.h"
#include "obj_loader.h"
#include "rasterization.h"
#include "tgaimage.h"
#include "utils.h"

constexpr int width = 64;
constexpr int height = 64;

constexpr TGAColor white    = { 255, 255, 255, 255 }; // attention, BGRA order
constexpr TGAColor red      = { 0,     0, 255, 255 };
constexpr TGAColor green    = { 0,   255,   0, 255 };
constexpr TGAColor blue     = { 255,   0,   0, 255 };
constexpr TGAColor cyan     = { 255, 255,   0, 255 };
constexpr TGAColor magenta  = { 255,   0, 255, 255 };
constexpr TGAColor yellow   = { 0,   255, 255, 255 };
constexpr TGAColor black    = { 0,     0,   0, 255 };

int main(int argc, char** argv) {
    TGAImage framebuffer(width, height, TGAImage::GRAYSCALE);

    int ax = 17, ay = 4, color_a = 15;
    int bx = 55, by = 39, color_b = 128;
    int cx = 23, cy = 59, color_c = 255;

    interpolate_modern_triangle(ax, ay, color_a, bx, by, color_b, cx, cy, color_c, framebuffer);

    //framebuffer.write_tga_file("output/framebuffer.tga");
    framebuffer.write_png_file("output/framebuffer.png");

    return 0;
}
