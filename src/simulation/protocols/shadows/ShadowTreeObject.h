#pragma once

#include "ShadowSimulationObject.h"

class ShadowTreeObject : public ShadowSimulationObject
{
private:
    friend class TreeObject;

    int _numberOfFruits;

public:
    ShadowTreeObject() {}
    ShadowTreeObject(unsigned long id_, Vec2<float> pos_,
                     int radius_, int numberOfFruits_)
        : ShadowSimulationObject(id_, pos_, radius_),
          _numberOfFruits(numberOfFruits_)
    {
    }

    ShadowSimulationObjectType type() const override { return ShadowSimulationObjectType::TreeObject; }

    int numberOfFruits() const { return _numberOfFruits; }
};