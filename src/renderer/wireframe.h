#pragma once

#include "framebuffer/framebuffer.h"
#include "framebuffer/tgaimage.h"
#include "graphics_mathematics/matrix.hpp"
#include "scene/camera.hpp"
#include "scene/scene.hpp"

void render_wireframe(const Scene&  scene,
                      const Camera& camera,
                      const mat4&   viewport_mat,
                      Framebuffer&  framebuffer,
                      TGAColor      color);
