#pragma once

#include "vector.hpp"

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
    [[nodiscard]] float det() const {
        static_assert(R == C);
        return recursive_det<C>::expand(*this);
    }

    // cofactor matrix (only square matrix)
    // (1) minor (T: 1 / F: 0)
    // (2) det(minor) * (-1)^{r + c}
    [[nodiscard]] float cofactor(int r, int c) const {
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
template<int R, int C> mat<R, C> operator*(mat<R, C> m, float s) {
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
template<int R, int C> mat<R, C> operator/(mat<R, C> m, float s) {
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
    static float expand(const mat<N, N>& m) {
        float sum = 0;
        for (int i = N; i--; sum += m[0][i] * m.cofactor(i, 0))
            ;
        return sum;
    }
};

template<> struct recursive_det<1>
{
    static float expand(const mat<1, 1>& m) { return m[0][0]; }
};

// type alias
using mat3 = mat<3, 3>;
using mat4 = mat<4, 4>;
