#pragma once

#include <random>
#include <cmath>

#include "protocols/brain/BotBrain.h"

class SurvivalBotBrain : public BotBrain
{
public:
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> dist = std::uniform_real_distribution<float>(-0.4f, 0.4f);
    float angle = 0.0f;

    /// @brief User defined brain class must have constructor that takes 0 arguments and use ": BotBrain()"
    SurvivalBotBrain() : BotBrain(), gen(rd()) {}

    void init() override 
    { // User defined brain class must have override init function that takes 0 arguments
        protocolsHolder->initProtocolResponce.r = 200;
        protocolsHolder->initProtocolResponce.g = 70;
        protocolsHolder->initProtocolResponce.b = 220;

        protocolsHolder->initProtocolResponce.healthPoints = 5;
        protocolsHolder->initProtocolResponce.foodPoints = 39;
        protocolsHolder->initProtocolResponce.visionPoints = 30;
        protocolsHolder->initProtocolResponce.speedPoints = 25;
        protocolsHolder->initProtocolResponce.attackPoints = 1;
    }

    

    void update() override
    { // User defined brain class must have override update function that takes 0 arguments
        // protocolsHolder->updateProtocolResponce.actionMove(Vec2<float>(-1.0f, -2.0f), 1.0f);
        // protocolsHolder->updateProtocolResponce.actionGoTo(Vec2<float>(200.0f, 500.0f));
        if (protocolsHolder->updateProtocol.distanceToNearestFood == -1.0f) {
            angle += dist(gen);
            angle = std::fmod(angle + M_PI, 2 * M_PI) - M_PI;
            protocolsHolder->updateProtocolResponce.actionMove(Vec2<float>(std::cos(angle), std::sin(angle)), 0.5f);
        }
        else {
            if (protocolsHolder->updateProtocol.distanceToNearestFood == 0.0f) {
                protocolsHolder->updateProtocolResponce.actionEatNearest();
            }
            else {
                protocolsHolder->updateProtocolResponce.actionGoTo(protocolsHolder->updateProtocol.nearestFood->pos());
            }
        }
    }

    void kill() override
    { // User defined brain class must have override kill function that takes 0 arguments
    }
};