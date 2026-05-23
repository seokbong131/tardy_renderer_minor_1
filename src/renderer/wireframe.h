#pragma once

#include "framebuffer/framebuffer.h"
#include "framebuffer/tgaimage.h"
#include "graphics_mathematics/matrix.hpp"
#include "scene/camera.hpp"
#include "scene/obj_loader.h"
#include "scene/scene.hpp"

void draw_mesh_wireframe(const Mesh&  mesh,
                         const mat4&  mvp_mat,
                         const mat4&  viewport_mat,
                         Framebuffer& framebuffer,
                         TGAColor     color);

void render_wireframe(const Scene&  scene,
                      const Camera& camera,
                      const mat4&   viewport_mat,
                      Framebuffer&  framebuffer,
                      TGAColor      color);
