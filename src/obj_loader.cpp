#include <fstream>
#include <sstream>
#include "obj_loader.h"

Mesh::Mesh(const std::string filename) {
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail()) return;

    std::string line;
    while (std::getline(in, line)) {
        std::istringstream iss(line.c_str());
        char slash;

        // vertex parsing
        if (!line.compare(0, 2, "v ")) {
            iss >> slash;
            vec3 v;

            for (int i : {0, 1, 2}) iss >> v[i];
            vertices.push_back(v);
        }

        // face parsing
        else if (!line.compare(0, 2, "f ")) {
            iss >> slash;
            int f, t, n, cnt = 0;

            // assumption
            // 1. fixed format: v/vt/vn
            // 2. offset by -1 (Wavefront .obj file: each index of face elements starts at 1)
            // 3. only triangle (NO quad, polygon, etc.)
            while (iss >> f >> slash >> t >> slash >> n) {
                indices.push_back(--f);
                cnt++;
            }

            if (3 != cnt) {
                std::cerr << "Error: the obj file is supposed to be triangulated" << std::endl;
                return;
            }
        }
    }

    std::cerr << "OBJ file loaded successfully." << std::endl;
    std::cerr << "[INFO] #(vertices): " << num_vertices() << ", #(triangles): " << num_triangles() << std::endl;
}

int Mesh::num_vertices() const { return static_cast<int>(vertices.size()); }
int Mesh::num_triangles() const { return static_cast<int>(indices.size() / 3); }

vec3 Mesh::get_vertex(const int i) const {
    return vertices[i];
}

vec3 Mesh::get_triangle_vertex(const int triangle_index, const int vertex_index_of_triangle) const {
    return vertices[indices[triangle_index * 3 + vertex_index_of_triangle]];
}
