#pragma once

#include <cassert>
#include <iostream>

// ------------------------------------------------------------
// vector
// ------------------------------------------------------------
template<int N> struct vec
{
    double data[N] = {};

    // i-th element
    constexpr double& operator[](int i) {
        assert(i >= 0 && i < N);
        return data[i];
    }
    constexpr double operator[](int i) const {
        assert(i >= 0 && i < N);
        return data[i];
    }
};

// addition (component-wise)
template<int N> constexpr vec<N> operator+(vec<N> a, const vec<N>& b) {
    for (int i = N; i--; a[i] += b[i])
        ;
    return a;
}

// subtraction (component-wise)
template<int N> constexpr vec<N> operator-(vec<N> a, const vec<N>& b) {
    for (int i = N; i--; a[i] -= b[i])
        ;
    return a;
}

// multiplication: vector x scalar (component-wise)
template<int N> constexpr vec<N> operator*(vec<N> v, double s) {
    for (int i = N; i--; v[i] *= s)
        ;
    return v;
}

// multiplication: scalar x vector (component-wise)
template<int N> constexpr vec<N> operator*(double s, vec<N> v) { return v * s; }

// division: vector / scalar (component-wise)
template<int N> constexpr vec<N> operator/(vec<N> v, double s) {
    for (int i = N; i--; v[i] /= s)
        ;
    return v;
}

// dot product (sum of component-wise multiplication)
template<int N> constexpr double operator*(const vec<N>& a, const vec<N>& b) {
    double sum = 0;
    for (int i = N; i--; sum += a[i] * b[i])
        ;
    return sum;
}

// stream output
template<int N> std::ostream& operator<<(std::ostream& out, const vec<N>& v) {
    for (int i = 0; i < N; ++i)
        out << v[i] << " ";
    return out;
}

// L2 norm: ||v||
template<int N> double norm(const vec<N>& v) { return std::sqrt(v * v); }

// normalization: v / ||v||
template<int N> vec<N> normalized(const vec<N>& v) { return v / norm(v); }

template<> struct vec<2>
{
    double x = 0, y = 0;

    constexpr double& operator[](int i) {
        assert(i >= 0 && i < 2);
        return i ? y : x;
    }
    constexpr double operator[](int i) const {
        assert(i >= 0 && i < 2);
        return i ? y : x;
    }
};

template<> struct vec<3>
{
    double x = 0, y = 0, z = 0;

    constexpr double& operator[](int i) {
        assert(i >= 0 && i < 3);
        return i ? (1 == i ? y : z) : x;
    }
    constexpr double operator[](int i) const {
        assert(i >= 0 && i < 3);
        return i ? (1 == i ? y : z) : x;
    }
};

// cross product: a × b (for vec3)
// perpendicular to both a and b
// magnitude: |a||b|sin(theta)
inline vec<3> cross(const vec<3>& a, const vec<3>& b) {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

template<> struct vec<4>
{
    double x = 0, y = 0, z = 0, w = 0;

    constexpr double& operator[](int i) {
        assert(i >= 0 && i < 4);
        return i < 2 ? (i ? y : x) : (2 == i ? z : w);
    }
    constexpr double operator[](int i) const {
        assert(i >= 0 && i < 4);
        return i < 2 ? (i ? y : x) : (2 == i ? z : w);
    }

    constexpr vec<2> xy() const { return {x, y}; }
    constexpr vec<3> xyz() const { return {x, y, z}; }
};

// type alias
using vec2 = vec<2>;
using vec3 = vec<3>;
using vec4 = vec<4>;

// forward declaration
template<int N> struct recursive_det;

// ------------------------------------------------------------
// matrix (column-major)
// ------------------------------------------------------------
template<int R, int C> struct mat
{
    vec<R> col[C] = {};

    // j-th column
    constexpr vec<R>& operator[](int j) {
        assert(j >= 0 && j < C);
        return col[j];
    }
    constexpr const vec<R>& operator[](int j) const {
        assert(j >= 0 && j < C);
        return col[j];
    }

    // i-th row -> vec<C>
    [[nodiscard]] constexpr vec<C> row(int i) const {
        vec<C> row_vec;
        for (int j = C; j--; row_vec[j] = col[j][i])
            ;
        return row_vec;
    }

    // determinant (only square matrix)
    // Ref. recursive_det<C>::expand
    [[nodiscard]] double det() const {
        static_assert(R == C);
        return recursive_det<C>::expand(*this);
    }

    // cofactor matrix (only square matrix)
    // (1) minor (T: 1 / F: 0)
    // (2) det(minor) * (-1)^{r + c}
    [[nodiscard]] double cofactor(int r, int c) const {
        assert(r >= 0 && r < R && c >= 0 && c < C);
        mat<R - 1, C - 1> minor;
        for (int i = R - 1; i--;)
            for (int j = C - 1; j--; minor[j][i] = col[j + (j >= c)][i + (i >= r)])
                ;
        return minor.det() * ((r + c) % 2 ? -1 : 1);
    }

    // transpose of the inverse matrix
    // (1) inverse matrix := 1 / det(M) * C^T (C: cofactor matrix)
    // (2) 1 / det(M) * C
    // Note. Laplace expansion
    // det(M) := sum of products of the entries in that row and their respective cofactors
    [[nodiscard]] mat inverse_transpose() const {
        mat cof_mat;
        for (int i = R; i--;)
            for (int j = C; j--; cof_mat[j][i] = cofactor(i, j))
                ;
        return cof_mat / (col[0] * cof_mat[0]);
    }

    // inverse matrix
    [[nodiscard]] mat inverse() const { return inverse_transpose().transpose(); }

    // transpose matrix
    [[nodiscard]] mat<C, R> transpose() const {
        mat<C, R> tp_mat;
        for (int i = R; i--;)
            for (int j = C; j--; tp_mat[i][j] = col[j][i])
                ;
        return tp_mat;
    }
};

// addition
template<int R, int C> mat<R, C> operator+(mat<R, C> a, const mat<R, C>& b) {
    for (int j = C; j--; a[j] = a[j] + b[j])
        ;
    return a;
}

// subtraction
template<int R, int C> mat<R, C> operator-(mat<R, C> a, const mat<R, C>& b) {
    for (int j = C; j--; a[j] = a[j] - b[j])
        ;
    return a;
}

// multiplication: matrix x scalar
template<int R, int C> mat<R, C> operator*(mat<R, C> m, double s) {
    for (int j = C; j--; m[j] = m[j] * s)
        ;
    return m;
}

// multiplication: matrix x vector
template<int R, int C> vec<R> operator*(const mat<R, C>& m, const vec<C>& v) {
    vec<R> col_vec;
    for (int i = R; i--; col_vec[i] = m.row(i) * v)
        ;
    return col_vec;
}

// multiplication: vector x matrix
template<int R, int C> vec<C> operator*(const vec<R>& v, const mat<R, C>& m) {
    vec<C> row_vec;
    for (int j = C; j--; row_vec[j] = v * m[j])
        ;
    return row_vec;
}

// multiplication: matrix x matrix
template<int R, int K, int C> mat<R, C> operator*(const mat<R, K>& a, const mat<K, C>& b) {
    mat<R, C> m;
    for (int j = C; j--; m[j] = a * b[j])
        ;
    return m;
}

// division: matrix / scalar
template<int R, int C> mat<R, C> operator/(mat<R, C> m, double s) {
    for (int j = C; j--; m[j] = m[j] / s)
        ;
    return m;
}

// stream output (row-major)
template<int R, int C> std::ostream& operator<<(std::ostream& out, const mat<R, C>& m) {
    for (int i = 0; i < R; ++i)
        out << m.row(i) << "\n";
    return out;
}

// expand -> cofactor -> det -> -> expand -> ...
template<int N> struct recursive_det
{
    static double expand(const mat<N, N>& m) {
        double sum = 0;
        for (int i = N; i--; sum += m[0][i] * m.cofactor(i, 0))
            ;
        return sum;
    }
};

template<> struct recursive_det<1>
{
    static double expand(const mat<1, 1>& m) { return m[0][0]; }
};
