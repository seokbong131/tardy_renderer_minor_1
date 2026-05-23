#include "rasterization.h"

#include "graphics_mathematics/vector.hpp"
#include "triangle.h"
#include "vertex_postprocessor.hpp"

// mesh-level (draw call)
void draw_mesh(const Mesh&  mesh,
               const mat4&  viewport_mat,
               Shader&      shader,
               Framebuffer& framebuffer) {
    for (int tri_id = 0; tri_id < mesh.num_triangles(); ++tri_id) {
        WindowCoord window_coord[3];

        // model-space -> clip-space -> window-space
        for (int v_id = 0; v_id < 3; ++v_id) {
            vec4 clip_coord    = shader.vertex(tri_id, v_id);
            window_coord[v_id] = postprocess_vertex(clip_coord, viewport_mat);
        }

        // skip the whole triangle (naive culling)
        if (window_coord[0].valid == false || window_coord[1].valid == false ||
            window_coord[2].valid == false)
            continue;

        draw_triangle(window_coord[0], window_coord[1], window_coord[2], framebuffer, shader);
    }
}

// scene-level (render pass)
void render(const Scene&  scene,
            const Camera& camera,
            const mat4&   viewport_mat,
            Shader&       shader,
            Framebuffer&  framebuffer) {
    mat4 view_mat       = view_matrix(camera);
    mat4 projection_mat = projection_matrix(camera);

    for (const auto& object : scene.objects) {
        // guard clause
        if (object.mesh == nullptr) continue;

        // set uniform, before each draw call
        shader.mesh    = object.mesh;
        shader.mvp_mat = projection_mat * view_mat * object.model_mat;

        draw_mesh(*object.mesh, viewport_mat, shader, framebuffer);
    }
}
