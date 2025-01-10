#pragma once

#include "protocols/brain/BotBrain.h"

class DummyBotBrain : public BotBrain
{
public:
    /// @brief User defined brain class must have constructor that takes 0 arguments
    DummyBotBrain() {}

    void init() override
    { // User defined brain class must have override init function that takes 0 arguments
        protocolsHolder->initProtocolResponce.r = 200;
        protocolsHolder->initProtocolResponce.g = 70;
        protocolsHolder->initProtocolResponce.b = 220;

        protocolsHolder->initProtocolResponce.healthPoints = 15;
        protocolsHolder->initProtocolResponce.foodPoints = 30;
        protocolsHolder->initProtocolResponce.visionPoints = 20;
        protocolsHolder->initProtocolResponce.speedPoints = 15;
        protocolsHolder->initProtocolResponce.attackPoints = 10;
    }

    void update() override
    { // User defined brain class must have override update function that takes 0 arguments
        protocolsHolder->updateProtocolResponce.actionMove(Vec2<float>(-1.0f, 1.0f), 0.6f);
    }

    void kill() override
    { // User defined brain class must have override kill function that takes 0 arguments
    }
};