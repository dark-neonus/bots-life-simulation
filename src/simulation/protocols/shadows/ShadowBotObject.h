#pragma once

#include "ShadowSimulationObject.h"

class ShadowBotObject : public ShadowSimulationObject {
private:
    friend class BotObject;

    

    float _health;
    float _food;
    int _seeDistance;
    float _speed;
    float _damage;

public:
    ShadowBotObject() {}
    ShadowBotObject(unsigned long id_, Vec2<float> pos_,
                    int health_, int food_,
                    int seeDistance_, int speed_,
                    int damage_)
        : ShadowSimulationObject(id_, pos_),
        _health(health_), _food(food_),
        _seeDistance(seeDistance_), _speed(speed_),
        _damage(damage_)
        {}

    float health() { return _health; }
    float food() { return _food; }
    int seeDistance() { return _seeDistance; }
    float speed() { return _speed; }
    float damage() { return _damage; }
};