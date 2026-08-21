#pragma once

#include <cmath>

namespace aethera {

struct Vec2 {
    float x{0.0f};
    float y{0.0f};

    constexpr Vec2() = default;
    constexpr Vec2(float x_, float y_) : x(x_), y(y_) {}

    constexpr Vec2 operator+(Vec2 rhs) const { return {x + rhs.x, y + rhs.y}; }
    constexpr Vec2 operator-(Vec2 rhs) const { return {x - rhs.x, y - rhs.y}; }
    constexpr Vec2 operator*(float s) const { return {x * s, y * s}; }
    constexpr Vec2 operator/(float s) const { return {x / s, y / s}; }

    Vec2& operator+=(Vec2 rhs) { x += rhs.x; y += rhs.y; return *this; }
    Vec2& operator-=(Vec2 rhs) { x -= rhs.x; y -= rhs.y; return *this; }
};

inline float dot(Vec2 a, Vec2 b) { return a.x * b.x + a.y * b.y; }
inline float length_squared(Vec2 v) { return dot(v, v); }
inline float length(Vec2 v) { return std::sqrt(length_squared(v)); }

inline Vec2 normalized(Vec2 v) {
    const float len = length(v);
    return len > 1e-6f ? v / len : Vec2{};
}

inline Vec2 lerp(Vec2 a, Vec2 b, float t) {
    return a + (b - a) * t;
}

inline float clamp(float value, float min_value, float max_value) {
    return value < min_value ? min_value : (value > max_value ? max_value : value);
}

} // namespace aethera
