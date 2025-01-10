#pragma once

#include "utilities/Vec2.h"

class ShadowSimulationObject {
private:
    friend class SimulationObject;

    unsigned long _id;

    Vec2<float> _pos;
public:
    ShadowSimulationObject() {}
    ShadowSimulationObject(unsigned long id_, Vec2<float> pos_)
        : _id(id_), _pos(pos_) {}

    unsigned long id() const { return _id; }

    Vec2<float> pos() const { return _pos; }
};