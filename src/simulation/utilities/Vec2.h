#pragma once


#include <cmath>
#include <iostream>
#include <stdexcept>
#include <array>
#include <type_traits>

template <typename T>
class Vec2
{
    static_assert(std::is_same<T, int>::value ||
                      std::is_same<T, float>::value ||
                      std::is_same<T, double>::value,
                  "Type not allowed!");

public:
    T x, y;

    // Constructors
    Vec2() : x(0), y(0) {}
    Vec2(T x, T y) : x(x), y(y) {}

    // Convert constructor
    template <typename U>
    Vec2(const Vec2<U> &other) : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) {}

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

    // Arithmetic operations


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
