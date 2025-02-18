#pragma once

#include "imgui.h"
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <array>
#include <type_traits>
#include <string>

template <typename T>
class Vec2
{
    static_assert(std::is_same<T, int>::value ||
                      std::is_same<T, float>::value ||
                      std::is_same<T, double>::value,
                  "Type not allowed!");

public:
    T x = static_cast<T>(0);
    T y = static_cast<T>(0);

    // Constructors
    Vec2() : x(0), y(0) {}
    Vec2(T x, T y) : x(x), y(y) {}

    // Convert constructor
    template <typename U>
    Vec2(const Vec2<U> &other) : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) {}
    Vec2(ImVec2 v) : x(static_cast<T>(v.x)), y(static_cast<T>(v.y)) {}

    static const Vec2<T> zero;

    // Direction vectors (individual directions)
    static const Vec2<T> right;
    static const Vec2<T> down;
    static const Vec2<T> left;
    static const Vec2<T> up;
    static const Vec2<T> right_up;
    static const Vec2<T> down_right;
    static const Vec2<T> down_left;
    static const Vec2<T> left_up;

    /// @return Length of current vector
    float length() const
    {
        return std::sqrt(x * x + y * y);
    }

    /// @brief Calculate the distance between this point and another point.
    /// @param other The other point to calculate the distance to.
    /// @return The distance between the two points.
    T distanceTo(const Vec2<T>& other) const {
        T dx = x - other.x;
        T dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    Vec2<T>& operator+=(const Vec2<T>& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    /// @brief Calculate the distance*distance between this point and another point(work faster than distanceTo())
    /// @param other The other point to calculate the distance to.
    /// @return The distance between the two points.
    T sqrDistanceTo(const Vec2<T>& other) const {
        T dx = x - other.x;
        T dy = y - other.y;
        return dx * dx + dy * dy;
    }

    /// @brief Normalize vector
    /// @return Vector of the same type and direction with length of 1
    Vec2<T> normalize() const
    {
        T len = length();
        if (len != 0)
        {
            return Vec2<T>(x / len, y / len);
        }
        return *this;
    }

    /// @brief Return dot product of vector and other: x * other.x + y * other.y
    /// @param Secong vector of dot production
    /// @return Dot production of current vector and other vector
    T dot(const Vec2<T> &other) const
    {
        return x * other.x + y * other.y;
    }

    // Arithmetic operations between vector and number
    template <typename T1, typename = std::enable_if_t<std::is_same<T1, int>::value ||
                                                       std::is_same<T1, float>::value ||
                                                       std::is_same<T1, double>::value>>
    Vec2<T> operator+(T1 number) const
    {
        return Vec2<T>(x + number, y + number);
    }

    template <typename T1, typename = std::enable_if_t<std::is_same<T1, int>::value ||
                                                       std::is_same<T1, float>::value ||
                                                       std::is_same<T1, double>::value>>
    Vec2<T> operator-(T1 number) const
    {
        return Vec2<T>(x - number, y - number);
    }

    template <typename T1, typename = std::enable_if_t<std::is_same<T1, int>::value ||
                                                       std::is_same<T1, float>::value ||
                                                       std::is_same<T1, double>::value>>
    Vec2<T> operator*(T1 number) const
    {
        return Vec2<T>(x * number, y * number);
    }

    template <typename T1, typename = std::enable_if_t<std::is_same<T1, int>::value ||
                                                       std::is_same<T1, float>::value ||
                                                       std::is_same<T1, double>::value>>
    Vec2<T> operator/(T1 number) const
    {
        if (number == 0)
        {
            throw std::invalid_argument("Division by zero");
        }
        return Vec2<T>(x / number, y / number);
    }

    // Arithmetic operations between vectors

    Vec2& operator/=(float scalar) {
        if (scalar != 0) {
            x /= scalar;
            y /= scalar;
        }
        else {
            throw std::runtime_error("Division by zero in Vec2.");
        }
        return *this;
    }

    Vec2<T> operator+(const Vec2<T> &other) const
    {
        return Vec2<T>(x + other.x, y + other.y);
    }

    Vec2<T> operator-(const Vec2<T> &other) const
    {
        return Vec2<T>(x - other.x, y - other.y);
    }

    Vec2<T> operator*(T scalar) const
    {
        return Vec2<T>(x * scalar, y * scalar);
    }

    Vec2<T> operator*(const Vec2<T> &other) const
    {
        return Vec2<T>(x * other.x, y * other.y);
    }

    Vec2<T> operator/(T scalar) const
    {
        if (scalar != 0)
        {
            return Vec2<T>(x / scalar, y / scalar);
        }
        throw std::invalid_argument("Can't divide by zero!");
    }

    // Comparison operations
    bool operator==(const Vec2<T> &other) const
    {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Vec2<T> &other) const
    {
        return !(*this == other);
    }

    bool operator>(const Vec2<T> &other) const
    {
        return x > other.x && y > other.y;
    }

    bool operator>=(const Vec2<T> &other) const
    {
        return x >= other.x && y >= other.y;
    }

    bool operator<(const Vec2<T> &other) const
    {
        return x < other.x && y < other.y;
    }

    bool operator<=(const Vec2<T> &other) const
    {
        return x <= other.x && y <= other.y;
    }

    // Output stream operator
    friend std::ostream &operator<<(std::ostream &os, const Vec2<T> &vec)
    {
        os << "Vec2<" << typeid(T).name() << ">(" << vec.x << ", " << vec.y << ")";
        return os;
    }

    std::string text() { return "Vec2<" + std::string(typeid(T).name()) + ">(" + std::to_string(x) + ", " + std::to_string(y) + ")"; }
};

// Static direction vectors (individual)
template <typename T>
const Vec2<T> Vec2<T>::right = Vec2<T>(1, 0);

template <typename T>
const Vec2<T> Vec2<T>::down = Vec2<T>(0, -1);

template <typename T>
const Vec2<T> Vec2<T>::left = Vec2<T>(-1, 0);

template <typename T>
const Vec2<T> Vec2<T>::up = Vec2<T>(0, 1);

template <typename T>
const Vec2<T> Vec2<T>::right_up = Vec2<T>(1, 1);

template <typename T>
const Vec2<T> Vec2<T>::down_right = Vec2<T>(1, -1);

template <typename T>
const Vec2<T> Vec2<T>::down_left = Vec2<T>(-1, -1);

template <typename T>
const Vec2<T> Vec2<T>::left_up = Vec2<T>(-1, 1);

// Arrays for 4 and 8 directions of integers
const std::array<Vec2<int>, 4> directions_4_int = {
    Vec2<int>(1, 0),  // right
    Vec2<int>(0, -1), // down
    Vec2<int>(-1, 0), // left
    Vec2<int>(0, 1)   // up
};

const std::array<Vec2<int>, 8> directions_8_int = {
    Vec2<int>(1, 0),   // right
    Vec2<int>(1, -1),  // down-right
    Vec2<int>(0, -1),  // down
    Vec2<int>(-1, -1), // down-left
    Vec2<int>(-1, 0),  // left
    Vec2<int>(-1, 1),  // up-left
    Vec2<int>(0, 1),   // up
    Vec2<int>(1, 1)    // up-right
};

// Arrays for 4 and 8 directions of floats
const std::array<Vec2<float>, 4> directions_4_float = {
    Vec2<float>(1.0f, 0.0f),  // right
    Vec2<float>(0.0f, -1.0f), // down
    Vec2<float>(-1.0f, 0.0f), // left
    Vec2<float>(0.0f, 1.0f)   // up
};

const std::array<Vec2<float>, 8> directions_8_float = {
    Vec2<float>(1.0f, 0.0f),   // right
    Vec2<float>(1.0f, -1.0f),  // down-right
    Vec2<float>(0.0f, -1.0f),  // down
    Vec2<float>(-1.0f, -1.0f), // down-left
    Vec2<float>(-1.0f, 0.0f),  // left
    Vec2<float>(-1.0f, 1.0f),  // up-left
    Vec2<float>(0.0f, 1.0f),   // up
    Vec2<float>(1.0f, 1.0f)    // up-right
};
