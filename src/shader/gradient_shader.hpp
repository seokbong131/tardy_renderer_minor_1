#pragma once

#include <cstdint>

#include "shader.hpp"

// [ NOTE ]
// now  : TGAColor (uint8_t BGRA)
// later: vec3 or vec4 (float) -> TGAColor conversion (clamp + 255 multiplication)
//
// barycentric interpolation of three vertex colors
struct GradientShader : Shader
{
    // v_out
    // vertex(...) write -> fragment(...) read
    TGAColor v_out[3]; // per-vertex

    // uniform
    // set by caller
    TGAColor vertex_colors[3] = {
        // default color (attention, BGRA order)
        // override per-instance if desired
        {0, 0, 0, 255},       // BLACK
        {0, 0, 255, 255},     // RED
        {255, 255, 255, 255}, // WHITE
    };

    vec4 vertex(int triangle_index, int vertex_index_of_triangle) override {
        v_out[vertex_index_of_triangle] = vertex_colors[vertex_index_of_triangle];

        vec3 v_position = mesh->get_triangle_vertex(triangle_index, vertex_index_of_triangle);

        return mvp_mat * vec4{v_position.x, v_position.y, v_position.z, 1.0f};
    }

    bool fragment(const vec3& barycentric_coord, TGAColor& output_color) override {
        // color interpolation
        for (int i = 0; i < 4; ++i) {
            output_color[i] = static_cast<std::uint8_t>(barycentric_coord.x * v_out[0][i] +
                                                        barycentric_coord.y * v_out[1][i] +
                                                        barycentric_coord.z * v_out[2][i]);
        }

        return true;
    }
};
