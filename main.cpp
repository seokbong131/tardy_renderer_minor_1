#include <format>

#include "config.h"
#include "obj_loader.h"
#include "shortcut.h"
#include "tgaimage.h"

int main(int argc, char** argv) {
    TGAImage framebuffer(WIDTH, HEIGHT, TGAImage::RGB);
    TGAImage zbuffer(WIDTH, HEIGHT, TGAImage::GRAYSCALE);

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
    // render_3(model, WIDTH, HEIGHT, BLACK, RED, WHITE, framebuffer);
    render_4(model, WIDTH, HEIGHT, zbuffer, framebuffer);

    zbuffer.write_png_file(std::format("{}/{}.png", OUTPUT_FOLDER, Z_BUFFER));
    framebuffer.write_png_file(std::format("{}/{}.png", OUTPUT_FOLDER, FRAME_BUFFER));

    /*volume_rendering_effect::visualize_slices(
        NUM_SLICES, model, WIDTH, HEIGHT, BLACK, RED, WHITE, framebuffer);*/

    return 0;
}
