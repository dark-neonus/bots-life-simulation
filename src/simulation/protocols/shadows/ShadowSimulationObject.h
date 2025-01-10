#pragma once

#include "utilities/Vec2.h"

enum ShadowSimulationObjectType
{
    BaseObject,
    FoodObject,
    TreeObject,
    BotObject
};

class ShadowSimulationObject
{
private:
    friend class SimulationObject;

    unsigned long _id;

    Vec2<float> _pos;

    int _radius;

public:
    ShadowSimulationObject() {}
    ShadowSimulationObject(unsigned long id_, Vec2<float> pos_, int radius_)
        : _id(id_), _pos(pos_), _radius(radius_) {}

    virtual ShadowSimulationObjectType type() const { return ShadowSimulationObjectType::BaseObject; }

    unsigned long id() const { return _id; }
    Vec2<float> pos() const { return _pos; }
    int radius() const { return _radius; }
};