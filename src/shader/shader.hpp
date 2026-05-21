#pragma once

#include "framebuffer/tgaimage.h"
#include "graphics_mathematics/matrix.hpp"
#include "graphics_mathematics/vector.hpp"
#include "scene/obj_loader.h"

// ----------------------------------------------------------------------------------------------------
// [ shader interface ]
// pipeline-side abstract base for the rasterizer
//
// [ shader variable ]
// - v_in   : input per vertex (from mesh, member variable X)
// - v_out  : output per vertex -> input per fragment (member variable O)
// - uniform: constant for the whole draw call (member variable O)
//
// [ NOTE ]
// modern GPU APIs pass vertex input structs.
// Ref 1. https://wikis.khronos.org/opengl/Vertex_Shader
// Ref 2. https://vulkan-tutorial.com/Vertex_buffers/Vertex_input_description
// Ref 3. https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-semantics
// Ref 4. https://developer.apple.com/metal/Metal-Shading-Language-Specification.pdf
// ----------------------------------------------------------------------------------------------------
struct Shader
{
    // built-in uniform
    const Mesh* mesh    = nullptr;
    mat4        mvp_mat = identity<4>();

    virtual ~Shader() = default;

    // vertex -> CC (vec4, since homogeneous)
    //
    // parameter:
    // 0 <= triangle_index < num_triangles()
    // 0 <= vertex_index_of_triangle < 3
    [[nodiscard]] virtual vec4 vertex(int triangle_index, int vertex_index_of_triangle) = 0;

    // barycentric coordinates of the pixel -> final color
    //
    // return:
    // true  => write output_color to framebuffer
    // false => discard
    [[nodiscard]] virtual bool fragment(const vec3& barycentric_coord, TGAColor& output_color) = 0;
};
