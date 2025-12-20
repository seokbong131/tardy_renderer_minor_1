#include "tgaimage.h"

constexpr TGAColor white    = { 255, 255, 255, 255 }; // attention, BGRA order
constexpr TGAColor red      = { 0,     0, 255, 255 };
constexpr TGAColor green    = { 0,   255,   0, 255 };
constexpr TGAColor blue     = { 255,   0,   0, 255 };
constexpr TGAColor cyan     = { 255, 255,   0, 255 };
constexpr TGAColor magenta  = { 255,   0, 255, 255 };
constexpr TGAColor yellow   = { 0,   255, 255, 255 };

static void draw_line(int start_x, int start_y, int end_x, int end_y, TGAImage& framebuffer, TGAColor color) {
    for (float t = 0.; t < 1.; t += .02) {
        int x = std::round(start_x + (end_x - start_x) * t);
        int y = std::round(start_y + (end_y - start_y) * t);
        framebuffer.set(x, y, color);
    }
}

int main(int argc, char** argv) {
    constexpr int width = 64;
    constexpr int height = 64;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    int ax = 7, ay = 3;
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
    framebuffer.set(cx, cy, white);

    framebuffer.write_tga_file("framebuffer.tga");
    framebuffer.write_png_file("framebuffer.png");
    return 0;
}
