#pragma once

#include "ShadowSimulationObject.h"

class ShadowFoodObject : public ShadowSimulationObject {
private:
    friend class FoodObject;

    float _calories;

    bool _isGrowing;
    bool _isDecaying;
public:
    ShadowFoodObject() {}
    ShadowFoodObject(unsigned long id_, Vec2<float> pos_,
                    float calories_, bool isGrowing_=false,
                    bool isDecaying_=false)
        : ShadowSimulationObject(id_, pos_),
        _calories(calories_), _isGrowing(isGrowing_),
        _isDecaying(isDecaying_) {}

    float calories() { return _calories; }
    bool isGrowing() { return _isGrowing; }
    bool isDecaying() { return _isDecaying; }
};