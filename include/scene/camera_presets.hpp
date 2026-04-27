#pragma once

#include "scene/camera.hpp"

// lecture reference
[[nodiscard]] inline Camera setup_default_camera() { return Camera{}; }

// only non-default override
[[nodiscard]] inline Camera set_custom_camera() {
    Camera camera;
    camera.eye = {1.5f, 1.0f, 3.0f};
    return camera;
}
