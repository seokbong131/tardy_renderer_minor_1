#include "config.h"
#include "obj_loader.h"
#include "shortcut.h"
#include "tgaimage.h"

int main(int argc, char** argv) {
    // model choice
    const auto& model_paths = DIABLO_PATH; // R
    // const auto& model_paths = BOGGIE_PATHS; // G
    // const auto& model_paths = AFRICAN_HEAD_PATHS; // B

    // mesh initialization by model path
    std::vector<Mesh> meshes;
    meshes.reserve(model_paths.size());
    for (const auto& path : model_paths) {
        meshes.emplace_back(path);
    }

    TGAImage framebuffer(WIDTH, HEIGHT, TGAImage::RGB);

    // rendering (rasterization)
    for (const auto& mesh : meshes) {
        render(mesh, WIDTH, HEIGHT, BLACK, RED, WHITE, framebuffer);
    }

    framebuffer.write_png_file("output/framebuffer.png");

    return 0;
}
