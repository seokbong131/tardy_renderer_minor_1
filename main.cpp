#include <format>

#include "config.h"
#include "obj_loader.h"
#include "rasterization.h"
#include "shortcut.h"
#include "tgaimage.h"
#include "utils.h"

int main(int argc, char** argv) {
    TGAImage framebuffer(WIDTH, HEIGHT, TGAImage::RGB);
    // the bigger, the farther (background)
    std::vector<float> depthbuffer(WIDTH * HEIGHT, std::numeric_limits<float>::max());

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

    // camera
    vec3 eye    = {-1.0, 0.0, 2.0};
    vec3 center = {0.0, 0.0, 0.0};
    vec3 up     = {0.0, 1.0, 0.0};

    double camera_distance = norm(eye - center);

    // matrix
    mat4 model_view = look_at(eye, center, up);
    mat4 projection = project_perspective(
        2.0 * std::atan(1.0 / camera_distance), static_cast<double>(WIDTH) / HEIGHT, 0.5, 4.0);
    mat4 viewport = transform_viewport(WIDTH / 16, HEIGHT / 16, WIDTH * 7 / 8, HEIGHT * 7 / 8);

    // rendering (rasterization)
    render_6(model,
             model_view,
             projection,
             viewport,
             WIDTH,
             HEIGHT,
             BLACK,
             RED,
             WHITE,
             depthbuffer,
             framebuffer);

    zbuffer_write_png_file(
        std::format("{}/{}.png", OUTPUT_FOLDER, Z_BUFFER), depthbuffer, WIDTH, HEIGHT);
    framebuffer.write_png_file(std::format("{}/{}.png", OUTPUT_FOLDER, FRAME_BUFFER));

    return 0;
}
