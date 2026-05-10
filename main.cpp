#include <format>

#include "config.hpp"
#include "framebuffer/framebuffer.h"
#include "framebuffer/framebuffer_io.h"
#include "graphics_mathematics/matrix_clip_space.hpp"
#include "obj_loader.h"
#include "renderer/viewport_presets.hpp"
#include "renderer/wireframe.h"
#include "scene/camera.hpp"
#include "scene/camera_presets.hpp"
#include "scene/scene.hpp"

int main() {
    // assets
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

    // scene
    Scene scene;
    scene.objects.push_back(SceneObject{&model});

    // camera
    Camera camera       = setup_default_camera(); // or set_custom_camera()
    camera.aspect_ratio = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);

    // framebuffer
    Framebuffer framebuffer = allocate(WIDTH, HEIGHT);
    clear(framebuffer, BLACK);

    // viewport matrix
    mat4 viewport_mat = setup_default_viewport(
        framebuffer.width, framebuffer.height); // or setup_fullscreen_viewport(...)

    render_wireframe(scene, camera, viewport_mat, framebuffer, RED);

    save_color_png(framebuffer, std::format("{}/{}.png", OUTPUT_FOLDER, COLOR_BUFFER));
    save_depth_png(framebuffer, std::format("{}/{}.png", OUTPUT_FOLDER, DEPTH_BUFFER));

    return 0;
}
