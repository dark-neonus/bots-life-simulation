#pragma once

#include "Vec2.h"
#include "RangeValue.h"
#include "GeneralFunctions.h"

class Camera {
private:
    RangeValue<float> x;
    RangeValue<float> y;

    float step;
public:
    Camera(float xLimit, float yLimit, float step_=5)
        : x(0.0, 0.0, xLimit), y(0.0, 0.0, yLimit), step(step_)
    {}

    /// @brief Update camera movement limits
    /// @param xLimit New x limit
    /// @param yLimit New y limit
    void updateLimits(float xLimit, float yLimit) {
        x.setMax(xLimit);
        y.setMax(yLimit);
    }

    /// @brief Set position for camera. Do automatical cut to bounds
    /// @param newPos New positiion of camera
    void setPos(Vec2<float> newPos) {
        x.set(newPos.x);
        y.set(newPos.y);
    }

    /// @brief Move camera to given direction
    /// @param dirX If positive move right, if negative move left, if zero dont move in X axis
    /// @param dirY If positive move down, if negative move up, if zero dont move in Y axis
    void move(int dirX, int dirY) {
        x.increase(signum(dirX) * step);
        y.increase(signum(dirY) * step);
    }

};