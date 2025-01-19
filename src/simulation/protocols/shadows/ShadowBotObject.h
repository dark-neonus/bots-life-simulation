#pragma once

#include <string>

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

    float _maxHealth;
    float _maxFood;

    std::string _populationName;

    bool _underAttack;
public:
    ShadowBotObject() {}
    ShadowBotObject(unsigned long id_, Vec2<float> pos_,
                    int radius_, float health_,
                    float food_, int seeDistance_,
                    float speed_, float damage_,
                    float maxHealth_, float maxFood_)
        : ShadowSimulationObject(id_, pos_, radius_),
          _health(health_),
          _food(food_),
          _seeDistance(seeDistance_),
          _speed(speed_),
          _damage(damage_),
          _maxHealth(maxHealth_),
          _maxFood(maxFood_),
          _underAttack(false)
    {
    }

    ShadowSimulationObjectType type() const override { return ShadowSimulationObjectType::ShadowBotObj; }

    float health() const { return _health; }
    float food() const { return _food; }
    int seeDistance() const { return _seeDistance; }
    float speed() const { return _speed; }
    float damage() const { return _damage; }

    float maxHealth() const { return _maxHealth; }
    float maxFood() const { return _maxFood; }
    bool isUnderAttack() const { return _underAttack; }

    std::string populationName() const { return _populationName; }
};