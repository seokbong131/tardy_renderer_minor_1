#pragma once

#include "framebuffer/framebuffer.h"
#include "graphics_mathematics/matrix.hpp"
#include "scene/camera.hpp"
#include "scene/scene.hpp"
#include "tgaimage.h"

void render_wireframe(const Scene&  scene,
                      const Camera& camera,
                      const mat4&   viewport_mat,
                      Framebuffer&  framebuffer,
                      TGAColor      color);
