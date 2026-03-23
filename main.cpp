#include <format>

#include "config.h"
#include "obj_loader.h"
#include "shortcut.h"
#include "tgaimage.h"
#include "utils.h"

int main(int argc, char** argv) {
    TGAImage framebuffer(WIDTH, HEIGHT, TGAImage::RGB);
    // the smaller, the farther (background)
    std::vector<float> depthbuffer(WIDTH * HEIGHT, std::numeric_limits<float>::lowest());

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
    render_5(model, WIDTH, HEIGHT, BLACK, RED, WHITE, depthbuffer, framebuffer);

    zbuffer_write_png_file(
        std::format("{}/{}.png", OUTPUT_FOLDER, Z_BUFFER), depthbuffer, WIDTH, HEIGHT);
    framebuffer.write_png_file(std::format("{}/{}.png", OUTPUT_FOLDER, FRAME_BUFFER));

    return 0;
}
