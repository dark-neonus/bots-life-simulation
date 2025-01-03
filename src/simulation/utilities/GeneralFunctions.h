#pragma once

#include "imgui.h"
#include "Vec2.h"
#define _USE_MATH_DEFINES
#include "math.h"

// Always use inline for functions defined in headers

constexpr float caloriesToSizeKoef = 1.0f;
/// @brief Convert calories to radius of circle that contain this amount of calories
/// @param calories Amount of calories
/// @param koef Koef to multiply radius by
/// @return Radius of circle containing given amount of calories
inline int convertCaloriesToRadius(int calories, float koef = caloriesToSizeKoef)
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
