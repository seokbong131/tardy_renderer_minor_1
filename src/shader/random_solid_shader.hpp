#pragma once

#include <cstdint>
#include <cstdlib>

#include "graphics_mathematics/matrix.hpp"
#include "obj_loader.h"
#include "shader.hpp"

// [ NOTE ]
// now  : TGAColor (uint8_t BGRA)
// later: vec3 or vec4 (float) -> TGAColor conversion (clamp + 255 multiplication)
//
// random solid color per triangle
struct RandomSolidShader : Shader
{
    // v_out
    // vertex(...) write -> fragment(...) read
    TGAColor v_out; // per-triangle

    // uniform
    // set by renderer, before each draw call
    const Mesh* mesh    = nullptr;
    mat4        mvp_mat = identity<4>();

    vec4 vertex(int triangle_index, int vertex_index_of_triangle) override {
        // new random solid color once per triangle (first vertex)
        if (vertex_index_of_triangle == 0) {
            for (int i = 0; i < 3; ++i) {
                v_out[i] = static_cast<std::uint8_t>(std::rand() % 256);
            }

            v_out[3] = 255;
        }

        vec3 v_position = mesh->get_triangle_vertex(triangle_index, vertex_index_of_triangle);

        return mvp_mat * vec4{v_position.x, v_position.y, v_position.z, 1.0f};
    }

    bool fragment(const vec3& /*barycentric_coord*/, TGAColor& output_color) override {
        output_color = v_out;

        return true;
    }
};
