#include "rasterization.h"

#include <algorithm>

// by Bresenham's line drawing algorithm
void draw_line(
    int start_x, int start_y, int end_x, int end_y, TGAImage& framebuffer, TGAColor color) {
    bool steep = std::abs(start_x - end_x) < std::abs(start_y - end_y);

    if (steep) { // transpose
        std::swap(start_x, start_y);
        std::swap(end_x, end_y);
    }

    if (start_x > end_x) { // right -> left
        std::swap(start_x, end_x);
        std::swap(start_y, end_y);
    }

    int y      = start_y;
    int ierror = 0;

    for (int x = start_x; x <= end_x; x++) {
        if (steep) // de-transpose
            framebuffer.set(y, x, color);
        else
            framebuffer.set(x, y, color);

        ierror += 2 * std::abs(end_y - start_y);

        if (ierror > end_x - start_x) {
            y += end_y > start_y ? 1 : -1;
            ierror -= 2 * (end_x - start_x);
        }
    }
}

// by shoelace formula
float compute_signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy) {
    return 0.5f * ((by - ay) * (bx + ax) + (cy - by) * (cx + bx) + (ay - cy) * (ax + cx));
}

// after this transformation,
//     camera position: the origin
//     camera direction: the negative Z-axis
// viewing transformation (M_v: WC -> EC)
// 1. translate: eye -> the origin
// 2. rotate: the camera frame -> eye space
// Ref. https://wikis.khronos.org/opengl/Viewing_and_Transformations
[[nodiscard]] mat4 look_at(const vec3& eye, const vec3& center, const vec3& rough_up) {
    // the camera frame (WC)
    vec3 backward = normalized(eye - center);              // the positive Z-axis
    vec3 right    = normalized(cross(rough_up, backward)); // the positive X-axis
    vec3 up       = cross(backward, right);                // the positive Y-axis

    //   C[0]       C[1]       C[2]       C[3]
    // | R.x        R.y        R.z        0    |
    // | U.x        U.y        U.z        0    |
    // | B.x        B.y        B.z        0    |
    // | 0          0          0          1    |
    mat4 rotation = {{{right.x, up.x, backward.x, 0.0},
                      {right.y, up.y, backward.y, 0.0},
                      {right.z, up.z, backward.z, 0.0},
                      {0.0, 0.0, 0.0, 1.0}}};

    //   C[0]   C[1]   C[2]   C[3]
    // | 1      0      0      -E.x |
    // | 0      1      0      -E.y |
    // | 0      0      1      -E.z |
    // | 0      0      0      1    |
    mat4 translation = {{{1.0, 0.0, 0.0, 0.0},
                         {0.0, 1.0, 0.0, 0.0},
                         {0.0, 0.0, 1.0, 0.0},
                         {-eye.x, -eye.y, -eye.z, 1.0}}};

    return rotation * translation;
}

// perspective projection (M_p: EC -> CC)
// Ref. https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/opengl-perspective-projection-matrix.html
[[nodiscard]] mat4 project_perspective(double fov_radian,
                                       double aspect_ratio,
                                       double near,
                                       double far) {
    // focal length
    double f = 1.0 / std::tan(fov_radian / 2.0);

    //   C[0]             C[1]             C[2]             C[3]
    // | f/a              0                0                0         |
    // | 0                f                0                0         |
    // | 0                0                (F+N)/(N-F)      2FN/(N-F) |
    // | 0                0                -1               0         | (*)
    mat4 projection = {{{f / aspect_ratio, 0.0, 0.0, 0.0},
                        {0.0, f, 0.0, 0.0},
                        {0.0, 0.0, (far + near) / (near - far), -1.0},
                        {0.0, 0.0, 2.0 * far * near / (near - far), 0.0}}};

    return projection;
}

// assumption: x and y specify the lower left corner.
// viewport transform (M_vp: NDC -> screen space)
// x: [-1, 1] -> [x, x + width - 1]
// y: [-1, 1] -> [y, y + height - 1]
// z: [-1, 1] -> [0, 1] (for depth buffer)
// Ref. https://registry.khronos.org/OpenGL-Refpages/gl4/html/glViewport.xhtml
[[nodiscard]] mat4 transform_viewport(int lower_left_corner_x,
                                      int lower_left_corner_y,
                                      int width,
                                      int height) {
    double half_width  = width * 0.5;
    double half_height = height * 0.5;

    //   C[0]       C[1]       C[2]       C[3]
    // | w/2        0          0          x + w/2 |
    // | 0          h/2        0          y + h/2 |
    // | 0          0          0.5        0.5     |
    // | 0          0          0          1       |
    mat4 viewport = {
        {{half_width, 0.0, 0.0, 0.0},
         {0.0, half_height, 0.0, 0.0},
         {0.0, 0.0, 0.5, 0.0},
         {lower_left_corner_x + half_width, lower_left_corner_y + half_height, 0.5, 1.0}}};

    return viewport;
}

#pragma warning(push)
#pragma warning(disable : 6993)
void draw_modern_triangle_with_f_depth(int                 ax,
                                       int                 ay,
                                       float               az,
                                       int                 bx,
                                       int                 by,
                                       float               bz,
                                       int                 cx,
                                       int                 cy,
                                       float               cz,
                                       int                 width,
                                       int                 height,
                                       std::vector<float>& depthbuffer,
                                       TGAImage&           framebuffer,
                                       TGAColor            color) {
    // screen-space AABB -> viewport clamping
    int aabb_min_x = std::max(std::min({ax, bx, cx}), 0);
    int aabb_min_y = std::max(std::min({ay, by, cy}), 0);
    int aabb_max_x = std::min(std::max({ax, bx, cx}), width - 1);
    int aabb_max_y = std::min(std::max({ay, by, cy}), height - 1);

    float total_area = compute_signed_triangle_area(ax, ay, bx, by, cx, cy);
    // total area < 0       => backface culling
    // total area = 0       => avoiding division by zero
    // 0 < total area < 1   => discarding triangles (< a pixel)
    if (total_area < 1) return;

#pragma omp parallel for
    for (int x = aabb_min_x; x <= aabb_max_x; x++) {
        for (int y = aabb_min_y; y <= aabb_max_y; y++) {
            // Area(PBC) := alpha, Area(PCA) := beta, Area(PAB) := gamma
            float alpha_area = compute_signed_triangle_area(x, y, bx, by, cx, cy) / total_area;
            float beta_area  = compute_signed_triangle_area(ax, ay, x, y, cx, cy) / total_area;
            float gamma_area = compute_signed_triangle_area(ax, ay, bx, by, x, y) / total_area;

            // Area > 0 => pixel is inside the triangle
            // Area = 0 => pixel is on the edge of the triangle
            // Area < 0 => pixel is outside the triangle
            if (alpha_area < 0 || beta_area < 0 || gamma_area < 0) continue;

            // depth interpolation
            float depth = alpha_area * az + beta_area * bz + gamma_area * cz;

            // depth test
            // the smaller, the nearer (=> update)
            // cast width to size_t to avoid an overflow issue
            if (depth >= depthbuffer[static_cast<std::size_t>(width) * y + x]) continue;

            depthbuffer[static_cast<std::size_t>(width) * y + x] = depth;
            framebuffer.set(x, y, color);
        }
    }
}
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 6993)
void interpolate_modern_triangle_with_f_depth(int                 ax,
                                              int                 ay,
                                              float               az,
                                              TGAColor            a_color,
                                              int                 bx,
                                              int                 by,
                                              float               bz,
                                              TGAColor            b_color,
                                              int                 cx,
                                              int                 cy,
                                              float               cz,
                                              TGAColor            c_color,
                                              int                 width,
                                              int                 height,
                                              std::vector<float>& depthbuffer,
                                              TGAImage&           framebuffer) {
    // screen-space AABB -> viewport clamping
    int aabb_min_x = std::max(std::min({ax, bx, cx}), 0);
    int aabb_min_y = std::max(std::min({ay, by, cy}), 0);
    int aabb_max_x = std::min(std::max({ax, bx, cx}), width - 1);
    int aabb_max_y = std::min(std::max({ay, by, cy}), height - 1);

    float total_area = compute_signed_triangle_area(ax, ay, bx, by, cx, cy);
    // total area < 0       => backface culling
    // total area = 0       => avoiding division by zero
    // 0 < total area < 1   => discarding triangles (< a pixel)
    if (total_area < 1) return;

#pragma omp parallel for
    for (int x = aabb_min_x; x <= aabb_max_x; x++) {
        for (int y = aabb_min_y; y <= aabb_max_y; y++) {
            // Area(PBC) := alpha, Area(PCA) := beta, Area(PAB) := gamma
            float alpha_area = compute_signed_triangle_area(x, y, bx, by, cx, cy) / total_area;
            float beta_area  = compute_signed_triangle_area(ax, ay, x, y, cx, cy) / total_area;
            float gamma_area = compute_signed_triangle_area(ax, ay, bx, by, x, y) / total_area;

            // Area > 0 => pixel is inside the triangle
            // Area = 0 => pixel is on the edge of the triangle
            // Area < 0 => pixel is outside the triangle
            if (alpha_area < 0 || beta_area < 0 || gamma_area < 0) continue;

            // depth interpolation
            float depth = alpha_area * az + beta_area * bz + gamma_area * cz;

            // depth test
            // the smaller, the nearer (=> update)
            // cast width to size_t to avoid an overflow issue
            if (depth >= depthbuffer[static_cast<std::size_t>(width) * y + x]) continue;

            depthbuffer[static_cast<std::size_t>(width) * y + x] = depth;

            TGAColor  color{};
            const int bpp = framebuffer.byte_per_pixel();

            for (int elem = 0; elem < bpp; ++elem) {
                color[elem] = static_cast<std::uint8_t>(alpha_area * a_color[elem] +
                                                        beta_area * b_color[elem] +
                                                        gamma_area * c_color[elem]);
            }

            framebuffer.set(x, y, color);
        }
    }
}
#pragma warning(pop)
