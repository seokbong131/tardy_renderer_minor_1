#include "tgaimage.h"

constexpr TGAColor white    = { 255, 255, 255, 255 }; // attention, BGRA order
constexpr TGAColor red      = { 0,     0, 255, 255 };
constexpr TGAColor green    = { 0,   255,   0, 255 };
constexpr TGAColor blue     = { 255,   0,   0, 255 };
constexpr TGAColor cyan     = { 255, 255,   0, 255 };
constexpr TGAColor magenta  = { 255,   0, 255, 255 };
constexpr TGAColor yellow   = { 0,   255, 255, 255 };

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

    // for performance measurement
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

    framebuffer.write_tga_file("output/framebuffer.tga");
    framebuffer.write_png_file("output/framebuffer.png");
    return 0;
}
