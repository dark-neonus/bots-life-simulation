#include <cmath>    // For std::lerp
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>   // For std::iota

/// @class PerlinNoise2D
/// @brief A class to compute 2D Perlin noise values.
class PerlinNoise2D {
private:
    /// @brief Gradient vectors used for Perlin noise computation.
    static const int grad2[8][2];

    /// @brief Permutation table for gradient selection.
    std::vector<int> permutation;

    /// @brief Smoothstep interpolation function.
    /// @param t The input value (should be in [0, 1]).
    /// @return The smoothed value.
    static float fade(float t) {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    /// @brief Computes the dot product of the gradient vector and the distance vector.
    /// @param ix Integer x-coordinate of the grid point.
    /// @param iy Integer y-coordinate of the grid point.
    /// @param x The x-coordinate of the input point.
    /// @param y The y-coordinate of the input point.
    /// @return The dot product value.
    float dotGridGradient(int ix, int iy, float x, float y) const {
        // Determine gradient index using the permutation table.
        int gradientIndex = permutation[(ix + permutation[iy % 256]) % 256] % 8;

        // Retrieve the gradient vector.
        const int* gradient = grad2[gradientIndex];

        // Compute distance vector.
        float dx = x - ix;
        float dy = y - iy;

        // Compute and return the dot product.
        return (gradient[0] * dx + gradient[1] * dy);
    }

    /// @brief Generates the permutation table.
    /// @return A vector containing the permutation table (duplicated to handle wraparound).
    static std::vector<int> generatePermutation() {
        std::vector<int> p(256);

        // Fill with values 0 to 255.
        std::iota(p.begin(), p.end(), 0);

        // Shuffle the values to create randomness.
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(p.begin(), p.end(), g);

        // Duplicate the table to avoid overflow during indexing.
        p.insert(p.end(), p.begin(), p.end());

        return p;
    }

public:
    /// @brief Constructor that initializes the permutation table.
    PerlinNoise2D() : permutation(generatePermutation()) {}

    /// @brief Computes 2D Perlin noise value at a given point.
    /// @param x The x-coordinate of the input point.
    /// @param y The y-coordinate of the input point.
    /// @return The Perlin noise value (range is typically [-1, 1]).
    float getValue(float x, float y) const {
        // Determine the integer coordinates of the grid cell containing the point.
        int x0 = static_cast<int>(std::floor(x));
        int x1 = x0 + 1;
        int y0 = static_cast<int>(std::floor(y));
        int y1 = y0 + 1;

        // Compute the local coordinates within the grid cell.
        float sx = x - x0;
        float sy = y - y0;

        // Compute fade curves for interpolation.
        float u = fade(sx);
        float v = fade(sy);

        // Compute dot products at the corners of the cell.
        float n00 = dotGridGradient(x0, y0, x, y);
        float n10 = dotGridGradient(x1, y0, x, y);
        float n01 = dotGridGradient(x0, y1, x, y);
        float n11 = dotGridGradient(x1, y1, x, y);

        // Interpolate along x for the bottom and top edges of the cell.
        float nx0 = std::lerp(n00, n10, u); // Bottom edge interpolation.
        float nx1 = std::lerp(n01, n11, u); // Top edge interpolation.

        // Interpolate along y for the final result.
        return std::lerp(nx0, nx1, v);
    }
};

// Define the gradient vectors.
const int PerlinNoise2D::grad2[8][2] = {
    {1, 1}, {-1, 1}, {1, -1}, {-1, -1},
    {1, 0}, {-1, 0}, {0, 1}, {0, -1}
};