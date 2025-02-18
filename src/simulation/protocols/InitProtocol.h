#pragma once

#include "utilities/Vec2.h"

struct InitProtocolResponce
{
    /// @brief Amount of points to spend on health parameter (health capacity)
    int healthPoints = 1;
    /// @brief Amount of points to spend on food parameter (calories capacity)
    int foodPoints = 1;
    /// @brief Amount of points to spend on vision parameter (distance of vision)
    int visionPoints = 1;
    /// @brief Amount of points to spend on speed parameter (max distance per movement)
    int speedPoints = 1;
    /// @brief Amount of points to spend on attack parameter (damage dealt by one hit)
    int attackPoints = 1;

    /// @brief Red value of bot's color
    int r = 0;
    /// @brief Green value of bot's color
    int g = 10;
    /// @brief Blue value of bot's color
    int b = 100;

    // Constructor
    InitProtocolResponce() {}
    InitProtocolResponce(
        int healthPoints_,
        int foodPoints_,
        int visionPoints_,
        int speedPoints_,
        int attackPoints_,
        int r_,
        int g_,
        int b_)
        : healthPoints(healthPoints_),
          foodPoints(foodPoints_),
          visionPoints(visionPoints_),
          speedPoints(speedPoints_),
          attackPoints(attackPoints_),
          r(r_),
          g(g_),
          b(b_) {}
};

struct InitProtocol
{
    /// @brief Position on which bot will be spawned
    Vec2<float> botSpawnPosition;
    /// @brief Maximal amount of evolution points that can be spent
    int evolutionPoints;
    ///
    int costumeMessage;

    // Constructor
    InitProtocol() {}
    InitProtocol(const Vec2<float> &botSpawnPosition_, int evolutionPoints_)
        : botSpawnPosition(botSpawnPosition_),
          evolutionPoints(evolutionPoints_) {}
};
