#pragma once

#include "utilities/Vec2.h"

class InitProtocolResponce {
public:
    int healthPoints;
    int foodPoints;
    int visionPoints;
    int speedPoints;
    int attackPoints;

    int r;
    int g;
    int b;
};

class InitProtocol {
public:
    Vec2<float> botSpawnPosition;
    int evolutionPoints;
};
