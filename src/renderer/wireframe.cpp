#include "wireframe.h"

#include "graphics_mathematics/vector.hpp"
#include "line.h"
#include "scene/obj_loader.h"
#include "vertex_postprocessor.hpp"

void render_wireframe(const Scene&  scene,
                      const Camera& camera,
                      const mat4&   viewport_mat,
                      Framebuffer&  framebuffer,
                      TGAColor      color) {
    mat4 view_mat       = view_matrix(camera);
    mat4 projection_mat = projection_matrix(camera);

    for (const auto& object : scene.objects) {
        // guard clause
        if (object.mesh == nullptr) continue;

        mat4 mvp_mat = projection_mat * view_mat * object.model_mat;

        for (int tri_id = 0; tri_id < object.mesh->num_triangles(); ++tri_id) {
            WindowCoord window_coord[3];

            // model-space -> clip-space -> window-space
            for (int v_id = 0; v_id < 3; ++v_id) {
                vec3 v_position = object.mesh->get_triangle_vertex(tri_id, v_id);
                vec4 clip_coord =
                    mvp_mat * vec4{v_position.x, v_position.y, v_position.z, 1.0f}; // homogeneous
                window_coord[v_id] = postprocess_vertex(clip_coord, viewport_mat);
            }

            // skip the whole triangle (naive culling)
            if (window_coord[0].valid == false || window_coord[1].valid == false ||
                window_coord[2].valid == false)
                continue;

            draw_line(window_coord[0].x,
                      window_coord[0].y,
                      window_coord[1].x,
                      window_coord[1].y,
                      framebuffer.color,
                      color);
            draw_line(window_coord[1].x,
                      window_coord[1].y,
                      window_coord[2].x,
                      window_coord[2].y,
                      framebuffer.color,
                      color);
            draw_line(window_coord[2].x,
                      window_coord[2].y,
                      window_coord[0].x,
                      window_coord[0].y,
                      framebuffer.color,
                      color);
        }
    }
}
