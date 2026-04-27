#include <format>
#include <iostream>

#include "config.h"
#include "obj_loader.h"
#include "scene/camera.hpp"
#include "scene/camera_presets.hpp"
#include "scene/scene.hpp"
#include "tgaimage.h"

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

    Scene scene;
    scene.objects.push_back(SceneObject{&model});

    Camera camera       = setup_default_camera(); // or set_custom_camera()
    camera.aspect_ratio = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);
    set_orbit(
        camera, /*center=*/{0.0f, 0.0f, 0.0f}, /*distance=*/3.0f, /*height=*/1.0f, /*theta=*/0.0f);

    std::cout << "\n[INFO] view matrix:\n" << view_matrix(camera);
    std::cout << "\n[INFO] projection matrix:\n" << projection_matrix(camera);

    return 0;
}
