#pragma once

struct EvolutionPointsSettings
{
    int amountOfPoints;

    float HealthForPoint;
    float FoodForPoint;
    int VisionDistanceForPoint;
    float SpeedForPoint;
    float DamageForPoint;

    float maxSeeDistanceSizeOfChunk;

    /// @brief Constructs EvolutionPointsSettings with default or provided values for all members.
    /// @param amountOfPoints_ Points available to spend (default: 100).
    /// @param HealthForPoint_ Points required to increase health by one unit (default: 1.0f).
    /// @param FoodForPoint_ Points required to increase food value by one unit (default: 1.0f).
    /// @param VisionDistanceForPoint_ Points required to increase vision distance by one unit (default: 10).
    /// @param SpeedForPoint_ Points required to increase speed by one unit (default: 1.0f).
    /// @param DamageForPoint_ Points required to increase damage by one unit (default: 1.0f).
    EvolutionPointsSettings(
        int amountOfPoints_ = 100,
        float HealthForPoint_ = 5.0f,
        float FoodForPoint_ = 5.0f,
        int VisionDistanceForPoint_ = 5,
        float SpeedForPoint_ = 0.3f,
        float DamageForPoint_ = 1.0f,
        float maxSeeDistanceSizeOfChunk_ = 0.95f)
        : amountOfPoints(amountOfPoints_),
          HealthForPoint(HealthForPoint_),
          FoodForPoint(FoodForPoint_),
          VisionDistanceForPoint(VisionDistanceForPoint_),
          SpeedForPoint(SpeedForPoint_),
          DamageForPoint(DamageForPoint_),
          maxSeeDistanceSizeOfChunk(maxSeeDistanceSizeOfChunk_) {}
    
    /// @brief Converts points to health value.
    /// @param points The number of points spent on health.
    /// @return The health value corresponding to the given points.
    float PointsToHealth(int points) const { return points * HealthForPoint; }

    /// @brief Converts points to food value.
    /// @param points The number of points spent on food.
    /// @return The food value corresponding to the given points.
    float PointsToFood(int points) const { return points * FoodForPoint; }

    /// @brief Converts points to vision distance.
    /// @param points The number of points spent on vision distance.
    /// @return The vision distance value corresponding to the given points.
    int PointsToVisionDistance(int points) const { return points * VisionDistanceForPoint; }

    /// @brief Converts points to speed.
    /// @param points The number of points spent on speed.
    /// @return The speed value corresponding to the given points.
    float PointsToSpeed(int points) const { return points * SpeedForPoint; }

    /// @brief Converts points to damage.
    /// @param points The number of points spent on damage.
    /// @return The damage value corresponding to the given points.
    float PointsToDamage(int points) const { return points * DamageForPoint; }
};