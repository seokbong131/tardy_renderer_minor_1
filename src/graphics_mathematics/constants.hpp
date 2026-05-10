#pragma once

#include <limits>
#include <numbers>

inline constexpr float EPSILON = std::numeric_limits<float>::epsilon();

inline constexpr float PI        = std::numbers::pi_v<float>;
inline constexpr float TWO_PI    = 2.0f * PI;
inline constexpr float HALF_PI   = 0.5f * PI;
inline constexpr float TO_RADIAN = PI / 180.0f;
inline constexpr float TO_DEGREE = 180.0f / PI;
