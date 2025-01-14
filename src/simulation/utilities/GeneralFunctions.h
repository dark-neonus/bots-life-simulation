#pragma once

#include "imgui.h"
#include "Vec2.h"
#define _USE_MATH_DEFINES
#include "math.h"
#include <algorithm>


// Always use inline for functions defined in headers

constexpr float caloriesToSizeKoef = 1.0f;
/// @brief Convert calories to radius of circle that contain this amount of calories
/// @param calories Amount of calories
/// @param koef Koef to multiply radius by
/// @return Radius of circle containing given amount of calories
inline int convertCaloriesToRadius(float calories, float koef = caloriesToSizeKoef)
{
    return int(ceil(sqrt(calories / M_PI) * koef));
}

/// @brief Convert channels values in range [0.0, 1.0] to color
/// @param r Red channel
/// @param g Green channel
/// @param b Blue channel
/// @param alpha Alpha channel (transparense)
/// @return ImVec4 with given values
inline ImVec4 colorFloat(float r, float g, float b, float alpha = 1.0f)
{
    return ImVec4(r, g, b, alpha);
}

/// @brief Convert channels values in range [0, 255] to color
/// @param r Red channel
/// @param g Green channel
/// @param b Blue channel
/// @param alpha Alpha channel (transparense)
/// @return ImVec4 with given values
inline ImVec4 colorInt(int r, int g, int b, int alpha = 255)
{
    return colorFloat(r / 255.0f, g / 255.0f, b / 255.0f, alpha / 255.0f);
}

/// @brief Convert Vec2<T> to ImVec2
/// @tparam T Type of Vec2
/// @param v Vec2 object to convert
/// @return ImVec2 with values from given Vec2 vector
template <typename T>
inline ImVec2 toImVec2(Vec2<T> v)
{
    return ImVec2(v.x, v.y);
}

/// @brief Return sign of given value
/// @tparam T Type value. Must be ether int, float or double
/// @param value Source of sign
/// @return 1 if value > 0, -1 if value < 0 and 0 if value == 0
template <typename T, typename = std::enable_if_t<std::is_same<T, int>::value ||
                                                       std::is_same<T, float>::value ||
                                                       std::is_same<T, double>::value>>
int signum(T value) {
    return (value > T(0)) - (value < T(0));
}

/// @brief Find point between p1 and p2 that lays on line on lines position t
/// @tparam T Type of vectors
/// @param p1 First point representing 0.0
/// @param p2 Second point representing 1.0
/// @param t The interpolation factor
/// @return Point resulted in interpolation
template <typename T>
Vec2<T> interpolate(const Vec2<T>& p1, const Vec2<T>& p2, T t) {
    // Ensure t is clamped between 0.0 and 1.0
    t = std::clamp(t, T(0.0), T(1.0));
    
    // Perform linear interpolation
    return Vec2<T>(
        p1.x + (p2.x - p1.x) * t,
        p1.y + (p2.y - p1.y) * t
    );
}