#pragma once

#include "ShadowSimulationObject.h"

class ShadowTreeObject : public ShadowSimulationObject {
private:
    friend class TreeObject;

    int _numberOfFruits;
public:
    ShadowTreeObject() {}
    ShadowTreeObject(unsigned long id_, Vec2<float> pos_, int numberOfFruits_)
        : ShadowSimulationObject(id_, pos_), 
        _numberOfFruits(numberOfFruits_)
        {}

    int numberOfFruits() { return _numberOfFruits; }
};