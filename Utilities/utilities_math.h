#pragma once

#include <cmath>


static constexpr float PI = 3.14159265358979323846f;
static constexpr float PI_OVER_TWO = PI / 2.0f;
static constexpr float DEG2RAD = PI / 180.0f;
static constexpr float RAD2DEG = 180.0f / PI;


struct Vector2D
{
    float x = 0.0f;
    float y = 0.0f;

    void print() const
    {
        std::cout << "(" << x << ", " << y << ")" << std::endl;
    }

    float sqr_norm() const
    {
        return x * x + y * y;
    }

    float to_angle() const
    {
        return std::atan2f(y, x);
    }

    float norm() const
    {
        return std::sqrtf(sqr_norm());
    }
    
    float norm1() const
    {
        return std::abs(x) + std::abs(y) ;
    }

    bool zero() const
    {
        return norm1() < 0.01f;
    }

    Vector2D rotate(float angle)
    {
        return { x * std::cos(angle) - y * std::sin(angle), x * std::sin(angle) + y * std::cos(angle) }; 
    }

    Vector2D normalize() const
    {
        float n = norm();
        return n != 0.0f ? Vector2D{ x / n, y / n } : Vector2D{ 0.0f, 0.0f };
    }

    Vector2D operator*(float scalar) const
    {
        return { x * scalar, y * scalar };
    }

    Vector2D operator*(int scalar) const
    {
        return { x * scalar, y * scalar };
    }

    Vector2D& operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Vector2D& operator*=(int scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Vector2D operator-() const
    {
        return { -x, -y };
    }

    Vector2D operator+(const Vector2D& v) const { return { x + v.x, y + v.y }; }
    Vector2D& operator+=(const Vector2D& v) { x += v.x; y += v.y; return *this; }
    Vector2D operator-(const Vector2D& v) const { return { x - v.x, y - v.y }; }
    Vector2D& operator-=(const Vector2D& v) { x -= v.x; y -= v.y; return *this; }

    std::string to_string() const
    {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }

    bool operator==(const Vector2D& v) const
    {
        return x == v.x && y == v.y;
    }

    bool operator!=(const Vector2D& v) const
    {
        return !(*this == v);
    }

    Vector2D& operator=(const Vector2D& v)
    {
        x = v.x;
        y = v.y;
        return *this;
    }
};

inline Vector2D operator*(float scalar, const Vector2D& v)
{
    return v * scalar;
}

inline Vector2D operator*(int scalar, const Vector2D& v)
{
    return v * scalar;
}

namespace Math
{
    inline float Lerp(float a, float b, float t)
    {
        return a + (b - a) * t;
    }

    inline float Dot(const Vector2D &u, const Vector2D &v)
    {
        return u.x * v.x + u.y * v.y;
    }

    inline float Abs(float f)
    {
        return f >= 0.0f ? f : -f;
    }

    inline float Clamp(float value, float min, float max)
    {
        if (value < min) value = min;
        else if (value > max) value = max;
        return value;
    }

    inline float Clamp01(float value)
    {
        return Clamp(value, 0, 1);
    }

    inline float AreNear(Vector2D v1, Vector2D v2, float distance)   
    {
        return (v2 - v1).sqr_norm() <= distance * distance;
    }
}
