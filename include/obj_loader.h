#pragma once

#include <vector>

#include "graphics_mathematics.h"

class Mesh
{
    std::vector<vec3> vertices  = {};
    double            min_depth = 0.0;
    double            max_depth = 0.0;
    std::vector<int>  indices   = {};

   public:
    Mesh(const std::string filename);
    int          num_vertices() const;
    const double get_min_depth() const;
    const double get_max_depth() const;
    int          num_triangles() const;
    vec3         get_vertex(const int i) const; // 0 <= i < num_vertices()
    vec3         get_triangle_vertex(
                const int triangle_index,                  // 0 <= triangle_index <= num_triangles()
                const int vertex_index_of_triangle) const; // 0 <= vertex_index_of_triangle < 3
};
