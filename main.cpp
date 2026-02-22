// #include <format>

#include "config.h"
#include "obj_loader.h"
#include "shortcut.h"
#include "tgaimage.h"

int main(int argc, char** argv) {
    TGAImage framebuffer(WIDTH, HEIGHT, TGAImage::RGB);

    // single-mesh model
    Mesh model(DIABLO_PATH); // R

    // multi-mesh model
    // ------------------------------------------------------------
    // const auto& model_paths = BOGGIE_PATHS; // G
    // const auto& model_paths = AFRICAN_HEAD_PATHS; // B

    /*std::vector<Mesh> meshes;
    meshes.reserve(model_paths.size());
    for (const auto& path : model_paths) {
        meshes.emplace_back(path);
    }
    Mesh model(meshes);*/
    // ------------------------------------------------------------

    // rendering (rasterization)
    // render(model, WIDTH, HEIGHT, BLACK, RED, WHITE, framebuffer);

    // framebuffer.write_png_file(std::format("{}/{}.png", OUTPUT_FOLDER, OUTPUT_FILE));

    volume_rendering_effect::visualize_slices(
        NUM_SLICES, model, WIDTH, HEIGHT, BLACK, RED, WHITE, framebuffer);

    return 0;
}
