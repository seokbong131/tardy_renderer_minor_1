#pragma once

#include "scene/camera.hpp"

// lecture reference
[[nodiscard]] inline Camera set_default_camera() { return Camera{}; }

// only non-default override
[[nodiscard]] inline Camera set_orthographic_camera() {
    Camera camera;
    camera.projection_type         = ProjectionType::Orthographic;
    camera.view_volume_half_height = 1.0f; // NOTE: adjust to match model scale. (WC)
    return camera;
}

// only non-default override
[[nodiscard]] inline Camera set_perspective_camera() {
    Camera camera;
    camera.eye = {1.5f, 1.0f, 3.0f};
    return camera;
}
