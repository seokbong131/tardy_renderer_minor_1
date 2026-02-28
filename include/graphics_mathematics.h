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
