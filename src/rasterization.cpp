#include "rasterization.h"

#include <algorithm>
#include <numbers>

// #include "config.h"

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

// assumption: x, y, and z are all in the range [-1, 1]. (WC)
// elevation: (x, y, z) -> (x, y) = 3D -> 2D
// viewport transform
// 1. x and y are all in the range [0, 2].
// 2. x and y are all in the range [0, 1]. (normalization)
// 3. x is in the range [0, width - 1] and y is in the range [0, height - 1]. (screen space)
std::tuple<int, int> project_orthographic_2(vec3 v, int width, int height) {
    // front (Z)
    int x = static_cast<int>(std::round((v.x + 1.0) * 0.5 * width));
    int y = static_cast<int>(std::round((v.y + 1.0) * 0.5 * height));

    // left (X)
    /*int x = static_cast<int>(std::round((-v.z + 1.0) * 0.5 * width));
    int y = static_cast<int>(std::round((v.y + 1.0) * 0.5 * height));*/

    // top (Y)
    /*int x = static_cast<int>(std::round((v.x + 1.0) * 0.5 * width));
    int y = static_cast<int>(std::round((-v.z + 1.0) * 0.5 * height));*/

    x = std::clamp(x, 0, width - 1);
    y = std::clamp(y, 0, height - 1);

    return {x, y};
}

// by scanline rendering algorithm
void draw_classic_triangle(
    int ax, int ay, int bx, int by, int cx, int cy, TGAImage& framebuffer, TGAColor color) {
    int x_coords[3] = {ax, bx, cx};
    int y_coords[3] = {ay, by, cy};

    // insertion sort by y-coordinate (ascending order)
    for (int i = 1; i < 3; ++i) {
        for (int j = i; j > 0 && y_coords[j] < y_coords[j - 1]; --j) {
            std::swap(x_coords[j], x_coords[j - 1]);
            std::swap(y_coords[j], y_coords[j - 1]);
        }
    }

    int total_height = y_coords[2] - y_coords[0];

    if (y_coords[0] != y_coords[1]) { // lower half
        int lower_height = y_coords[1] - y_coords[0];

        // T := upper half (X) -> include y_coords[1] scanline
        // F := upper half (O) -> avoid overlap
        int y_end = (y_coords[1] == y_coords[2]) ? y_coords[1] : y_coords[1] - 1;

        for (int y = y_coords[0]; y <= y_end; y++) {
            // linear interpolation (integer arithmetic)
            int x_1 = x_coords[0] +
                      ((x_coords[1] - x_coords[0]) * (y - y_coords[0])) / lower_height;
            int x_2 = x_coords[0] +
                      ((x_coords[2] - x_coords[0]) * (y - y_coords[0])) / total_height;

            // top-left rule (X) -> overlap
            for (int x = std::min(x_1, x_2); x <= std::max(x_1, x_2); x++)
                framebuffer.set(x, y, color);
        }
    }

    if (y_coords[1] != y_coords[2]) { // upper half
        int upper_height = y_coords[2] - y_coords[1];

        for (int y = y_coords[1]; y <= y_coords[2]; y++) {
            // linear interpolation (integer arithmetic)
            int x_1 = x_coords[1] +
                      ((x_coords[2] - x_coords[1]) * (y - y_coords[1])) / upper_height;
            int x_2 = x_coords[0] +
                      ((x_coords[2] - x_coords[0]) * (y - y_coords[0])) / total_height;

            // top-left rule (X) -> overlap
            for (int x = std::min(x_1, x_2); x <= std::max(x_1, x_2); x++)
                framebuffer.set(x, y, color);
        }
    }
}

// by shoelace formula
float compute_signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy) {
    return 0.5f * ((by - ay) * (bx + ax) + (cy - by) * (cx + bx) + (ay - cy) * (ax + cx));
}

#pragma warning(push)
#pragma warning(disable : 6993)
void draw_modern_triangle(
    int ax, int ay, int bx, int by, int cx, int cy, TGAImage& framebuffer, TGAColor color) {
    int aabb_min_x = std::min(std::min(ax, bx), cx);
    int aabb_min_y = std::min(std::min(ay, by), cy);
    int aabb_max_x = std::max(std::max(ax, bx), cx);
    int aabb_max_y = std::max(std::max(ay, by), cy);

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

            framebuffer.set(x, y, color);
        }
    }
}
#pragma warning(pop)

// by digital differential analyzer (DDA) algorithm <= 2nd attempt, round 1
void draw_temporary_line(
    int start_x, int start_y, int end_x, int end_y, TGAImage& framebuffer, TGAColor color) {
    for (int x = start_x; x <= end_x; x++) {
        float t = (x - start_x) / static_cast<float>(end_x - start_x);
        int   y = static_cast<int>(std::lround(start_y + (end_y - start_y) * t));
        framebuffer.set(x, y, color);
    }
}

#pragma warning(push)
#pragma warning(disable : 6993)
void interpolate_modern_triangle(int       ax,
                                 int       ay,
                                 TGAColor  a_color,
                                 int       bx,
                                 int       by,
                                 TGAColor  b_color,
                                 int       cx,
                                 int       cy,
                                 TGAColor  c_color,
                                 TGAImage& framebuffer) {
    int aabb_min_x = std::min(std::min(ax, bx), cx);
    int aabb_min_y = std::min(std::min(ay, by), cy);
    int aabb_max_x = std::max(std::max(ax, bx), cx);
    int aabb_max_y = std::max(std::max(ay, by), cy);

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

            // wireframe effect
            /*if (alpha_area > 0.1f && beta_area > 0.1f && gamma_area > 0.1f)
                continue;*/

            // visualization
            // if (alpha_area > 0.1f) framebuffer.set(x, y, MAGENTA);
            // if (beta_area > 0.1f) framebuffer.set(x, y, YELLOW);
            // if (gamma_area > 0.1f) framebuffer.set(x, y, CYAN);

            /* if (alpha_area > 0.1f)
            {
                if (beta_area > 0.1f) {
                    if (gamma_area > 0.1f)  framebuffer.set(x, y, WHITE);
                    else                    framebuffer.set(x, y, PINK);
                }
                else {
                    if (gamma_area > 0.1f)  framebuffer.set(x, y, SKY_BLUE);
                    else                    framebuffer.set(x, y, MAGENTA);
                }
            }
            else {
                if (beta_area > 0.1f) {
                    if (gamma_area > 0.1f)  framebuffer.set(x, y, MINT);
                    else                    framebuffer.set(x, y, YELLOW);
                }
                else {
                    if (gamma_area > 0.1f)  framebuffer.set(x, y, CYAN);
                    else                    continue;
                }
            }*/

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

#pragma warning(push)
#pragma warning(disable : 6993)
void draw_modern_triangle_with_i_depth(int       ax,
                                       int       ay,
                                       int       az,
                                       int       bx,
                                       int       by,
                                       int       bz,
                                       int       cx,
                                       int       cy,
                                       int       cz,
                                       TGAImage& zbuffer,
                                       TGAImage& framebuffer,
                                       TGAColor  color) {
    int aabb_min_x = std::min(std::min(ax, bx), cx);
    int aabb_min_y = std::min(std::min(ay, by), cy);
    int aabb_max_x = std::max(std::max(ax, bx), cx);
    int aabb_max_y = std::max(std::max(ay, by), cy);

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
            uint8_t depth = static_cast<std::uint8_t>(alpha_area * az + beta_area * bz +
                                                      gamma_area * cz);

            // per-pixel painter's algorithm
            // 0 (far) <-> 255 (near => update)
            auto z_value = zbuffer.get(x, y)[0];
            if (depth <= z_value) continue;

            zbuffer.set(x, y, {depth});
            framebuffer.set(x, y, color);
        }
    }
}
#pragma warning(pop)

// assumption: x, y, and z are all in the range [-1, 1]. (WC)
// elevation: (x, y, z) -> (x, y) = 3D -> 2D
// viewport transform
// 1. x and y are all in the range [0, 2].
// 2. x and y are all in the range [0, 1]. (normalization)
// 3. x is in the range [0, width - 1] and y is in the range [0, height - 1]. (screen space)
// 4. z is in the range [0, 255]. (screen space, variable)
std::tuple<int, int, int> project_orthographic_3(vec3 v, int width, int height) {
    // front (Z)
    int x = static_cast<int>(std::round((v.x + 1.0) * 0.5 * width));
    int y = static_cast<int>(std::round((v.y + 1.0) * 0.5 * height));
    int z = static_cast<int>(std::round((v.z + 1.0) * 0.5 * 255.0));

    // left (X)
    /*int x = static_cast<int>(std::round((-v.z + 1.0) * 0.5 * width));
    int y = static_cast<int>(std::round((v.y + 1.0) * 0.5 * height));
    int z = static_cast<int>(std::round((v.x + 1.0) * 0.5 * 255.0));*/

    // top (Y)
    /*int x = static_cast<int>(std::round((v.x + 1.0) * 0.5 * width));
    int y = static_cast<int>(std::round((-v.z + 1.0) * 0.5 * height));
    int z = static_cast<int>(std::round((v.y + 1.0) * 0.5 * 255.0));*/

    x = std::clamp(x, 0, width - 1);
    y = std::clamp(y, 0, height - 1);
    z = std::clamp(z, 0, 255);

    return {x, y, z};
}

vec3 rotate_naive(const vec3& v) {
    static constexpr double theta      = std::numbers::pi / 6.0;
    static const mat<3, 3>  rotation_y = {{{std::cos(theta), 0.0, -std::sin(theta)},
                                           {0.0, 1.0, 0.0},
                                           {std::sin(theta), 0.0, std::cos(theta)}}};
    return rotation_y * v;
}

// assumption: camera position is (0, 0, c).
vec3 project_perspective_naive(const vec3& v) {
    static constexpr double c = 3.0;
    return v / (1.0 - v.z / c);
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
                                       std::vector<float>& depthbuffer,
                                       TGAImage&           framebuffer,
                                       TGAColor            color) {
    int aabb_min_x = std::min(std::min(ax, bx), cx);
    int aabb_min_y = std::min(std::min(ay, by), cy);
    int aabb_max_x = std::max(std::max(ax, bx), cx);
    int aabb_max_y = std::max(std::max(ay, by), cy);

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
                                              std::vector<float>& depthbuffer,
                                              TGAImage&           framebuffer) {
    int aabb_min_x = std::min(std::min(ax, bx), cx);
    int aabb_min_y = std::min(std::min(ay, by), cy);
    int aabb_max_x = std::max(std::max(ax, bx), cx);
    int aabb_max_y = std::max(std::max(ay, by), cy);

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
