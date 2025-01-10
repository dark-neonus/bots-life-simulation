#pragma once

#include "ShadowSimulationObject.h"

class ShadowFoodObject : public ShadowSimulationObject
{
private:
    friend class FoodObject;

    float _calories;

    bool _isGrowing;
    bool _isDecaying;

public:
    ShadowFoodObject() {}
    ShadowFoodObject(unsigned long id_, Vec2<float> pos_,
                     int radius_, float calories_,
                     bool isGrowing_ = false, bool isDecaying_ = false)
        : ShadowSimulationObject(id_, pos_, radius_),
          _calories(calories_), _isGrowing(isGrowing_),
          _isDecaying(isDecaying_) {}

    ShadowSimulationObjectType type() const override { return ShadowSimulationObjectType::ShadowFoodObj; }

    float calories() const { return _calories; }
    bool isGrowing() const { return _isGrowing; }
    bool isDecaying() const { return _isDecaying; }
};