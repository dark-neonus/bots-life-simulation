#pragma once

#include "ShadowSimulationObject.h"

class ShadowBotObject : public ShadowSimulationObject
{
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
                    int radius_, int health_,
                    int food_, int seeDistance_,
                    int speed_, int damage_)
        : ShadowSimulationObject(id_, pos_, radius_),
          _health(health_), _food(food_),
          _seeDistance(seeDistance_), _speed(speed_),
          _damage(damage_)
    {
    }

    ShadowSimulationObjectType type() const override { return ShadowSimulationObjectType::BotObject; }

    float health() const { return _health; }
    float food() const { return _food; }
    int seeDistance() const { return _seeDistance; }
    float speed() const { return _speed; }
    float damage() const { return _damage; }
};