#pragma once

#include "imgui.h"

#include "Vec2.h"
#include "RangeValue.h"
#include "GeneralFunctions.h"

class Camera {
private:
    float _xLimit;
    float _yLimit;

    RangeValue<float> _x;
    RangeValue<float> _y;

    RangeValue<float> _width;
    RangeValue<float> _height;

    bool movingToDestination = false;
    Vec2<float> destination;

    bool isMoving = false;
    float speedMultyplier = 1.0f;
public:
    Camera() {};
    Camera(float xLimit, float yLimit, float step_=5)
        : _xLimit(xLimit), _yLimit(yLimit),
        _x(0.0, 0.0, _xLimit), _y(0.0, 0.0, _yLimit),
        _width(0.0, 0.0, _xLimit), _height(0.0, 0.0, _yLimit)
    {}

    float x() { return _x.get(); }
    float y() { return _y.get(); }
    float width() { return _width.get(); }
    float height() { return _height.get(); }

    float getXLimit() { return _x.getMax(); }
    float getYLimit() { return _y.getMax(); }

    // /// @brief Update camera movement limits
    // /// @param xLimit New x limit
    // /// @param yLimit New y limit
    // void updateLimits(float xLimit, float yLimit) {
    //     _x.setMax(xLimit);
    //     _y.setMax(yLimit);
    // }

    /// @brief Resize camera. Automatically handle out of limits case
    /// @param newWidth New width
    /// @param newHeight New height
    void setSize(float newWidth, float newHeight) {
        _width.set(newWidth);
        _height.set(newHeight);
        _x.setMax(_xLimit - _width.get());
        _y.setMax(_yLimit - _height.get());
    }

    /// @brief Set position for camera. Do automatical cut to bounds
    /// @param newPos New positiion of camera
    void setPos(Vec2<float> newPos) {
        _x.set(newPos.x);
        _y.set(newPos.y);
    }

    /// @brief Move camera to given direction
    /// @param dirX If positive move right, if negative move left, if zero dont move in X axis
    /// @param dirY If positive move down, if negative move up, if zero dont move in Y axis
    void move(int dirX, int dirY, float step = 5.0f) {
        _x.increase(signum(dirX) * step * speedMultyplier);
        _y.increase(signum(dirY) * step * speedMultyplier);
        speedMultyplier = std::min(3.0f, speedMultyplier + 0.01f);
        isMoving = true;
    }

    /// @brief Draw X and Y controls for camera position on current ImGui window
    void drawCameraControls() {
        ImGui::SliderFloat("CameraPosX", _x.valuePointer(), _x.getMin(), _x.getMax(), "%.1f");
        ImGui::SliderFloat("CameraPosY", _y.valuePointer(), _y.getMin(), _y.getMax(), "%.1f");
    }

    bool isPointInVision(Vec2<float> point) {
        return (
            point.x >= x() && point.x <= x() + width() &&
            point.y >= y() && point.y <= y() + height()
        );
    }

    void update() {
        if (movingToDestination) {
            if (destination.distanceTo(Vec2<float>(x(), y())) > 30) {
                Vec2<float> nextPos = interpolate<float>(Vec2<float>(x(), y()), destination, 0.3);
                _x.set(nextPos.x);
                _y.set(nextPos.y);
            }
            else {
                _x.set(destination.x);
                _y.set(destination.y);
                movingToDestination = false;
            }
        }
        if (!isMoving) {
            speedMultyplier = 1.0f;
        }
        isMoving = false;
    }

    void moveTo(Vec2<float> destination_) {
        movingToDestination = true;
        float destinationX = std::min(_x.getMax(), std::max(_x.getMin(), destination_.x - width()));
        float destinationY = std::min(_y.getMax(), std::max(_y.getMin(), destination_.y - height()));
        destination = Vec2<float>(destinationX, destinationY);
    }

    /// @brief Returns the top-left corner coordinates of the camera.
    Vec2<float> getTopLeft() {
        return Vec2<float>(x(), y());
    }

    /// @brief Returns the bottom-right corner coordinates of the camera.
    Vec2<float> getBottomRight() {
        return Vec2<float>(x() + width(), y() + height());
    }
};