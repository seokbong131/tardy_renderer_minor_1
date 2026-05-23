#pragma once

#include "framebuffer/framebuffer.h"
#include "graphics_mathematics/matrix.hpp"
#include "scene/camera.hpp"
#include "scene/obj_loader.h"
#include "scene/scene.hpp"
#include "shader/shader.hpp"

void draw_mesh(const Mesh&  mesh,
               const mat4&  viewport_mat,
               Shader&      shader,
               Framebuffer& framebuffer);

void render(const Scene&  scene,
            const Camera& camera,
            const mat4&   viewport_mat,
            Shader&       shader,
            Framebuffer&  framebuffer);
