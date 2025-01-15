#pragma once

#include <random>
#include <cmath>
#include <memory>

#include "protocols/brain/BotBrain.h"
#include "protocols/brain/BrainsRegistry.h"


class AggressiveMultiplierBotBrain : public BotBrain
{
private:
    static int population;
    static int death;
    static unsigned long b_id;
    void printStats() {
        std::cout << "<" << populationName;
        std::cout << "> |- population: " << population;
        std::cout << " | death: " << death;
        std::cout << " | b_id: " << b_id << " |\n";
    }
public:
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> deltaAngle = std::uniform_real_distribution<float>(-0.4f, 0.4f);
    std::uniform_real_distribution<float> circleAngle = std::uniform_real_distribution<float>(-M_PI, M_PI);
    std::uniform_int_distribution<int> randomDirChance = std::uniform_int_distribution<int>(0, 7);
    float angle = 0.0f;

    /// @brief User defined brain class must have constructor that takes 0 arguments and use ": BotBrain(populationName)"
    AggressiveMultiplierBotBrain() 
    : BotBrain("AgressiveMultiplierLegion")
    {
    }

    void init() override 
    { // User defined brain class must have override init function that takes 0 arguments
        protocolsHolder->initProtocolResponce.r = 210;
        protocolsHolder->initProtocolResponce.g = 130;
        protocolsHolder->initProtocolResponce.b = 30;

        protocolsHolder->initProtocolResponce.healthPoints = int(0.05 * protocolsHolder->initProtocol.evolutionPoints);
        protocolsHolder->initProtocolResponce.foodPoints = int(0.25 * protocolsHolder->initProtocol.evolutionPoints);
        protocolsHolder->initProtocolResponce.visionPoints = int(0.37 * protocolsHolder->initProtocol.evolutionPoints);
        protocolsHolder->initProtocolResponce.speedPoints = int(0.25 * protocolsHolder->initProtocol.evolutionPoints);
        protocolsHolder->initProtocolResponce.attackPoints = int(0.07 * protocolsHolder->initProtocol.evolutionPoints);

        angle = circleAngle(gen);
        population++;
        b_id++;
        gen = std::mt19937(static_cast<std::mt19937::result_type>(rd() + b_id));
        printStats();
        // std::cout << "random_seq: ";
        // for (int i = 0 ; i< 10; i++) {
        // std::cout << " " << randomDirChance(gen);
        // }
        // std::cout << "\n";
    }

    int focusTime = 0;
    int bornAmount = 0;

    std::weak_ptr<const ShadowSimulationObject> focusedFood;

    void update() override
    { // User defined brain class must have override update function that takes 0 arguments
        // std::cout << "process_bot_: " << protocolsHolder->updateProtocol.body->id() << "\n";
        // if (bornAmount >= 2 && population > 120 || bornAmount >= 1 && population > 500) {
        //     protocolsHolder->updateProtocolResponce.actionSuicide();
        //     return;
        // }
        if (seeEnemy() &&
            // protocolsHolder->updateProtocol.nearestEnemy->speed() + 2 < protocolsHolder->updateProtocol.body->speed() &&
            protocolsHolder->updateProtocol.body->food() >= 0.6f * protocolsHolder->updateProtocol.body->maxFood()) {
            if (canReach(protocolsHolder->updateProtocol.nearestEnemy)) {
                protocolsHolder->updateProtocolResponce.actionAttackByID(false, protocolsHolder->updateProtocol.nearestEnemy->id());
            }
            else {
                protocolsHolder->updateProtocolResponce.actionGoTo(protocolsHolder->updateProtocol.nearestEnemy->pos());
            }
            return;
        }
        if (auto validFocusedFood = focusedFood.lock()) {
            if (canReach(validFocusedFood)) {
                if (protocolsHolder->updateProtocol.body->food() >= 0.9f * protocolsHolder->updateProtocol.body->maxFood() &&
                    protocolsHolder->updateProtocol.body->health() == protocolsHolder->updateProtocol.body->maxHealth()) {
                    protocolsHolder->updateProtocolResponce.actionSpawn(
                        std::dynamic_pointer_cast<BotBrain>(
                            std::make_shared<AggressiveMultiplierBotBrain>()
                        ),
                        protocolsHolder->initProtocol.evolutionPoints + 5
                    );
                    bornAmount++;
                }
                else {
                    // std::cout << "try to eat: " << validFocusedFood->id() << "\n";
                    // std::cout << "myPos: " << protocolsHolder->updateProtocol.body->pos().text() << "targetPos: " << validFocusedFood->pos() << "\n";
                    protocolsHolder->updateProtocolResponce.actionEatByID(validFocusedFood->id());
                }
            }
            else {
                protocolsHolder->updateProtocolResponce.actionGoTo(validFocusedFood->pos());
            }
        }
        else {
            int indexOfObject = randomDirChance(gen);
            for (auto& obj : protocolsHolder->updateProtocol.visibleObjects) {
                if (obj->type() == ShadowFoodObj) {
                    if (indexOfObject <= 0) {
                        focusedFood = obj;
                        // std::cout << "bot_" << protocolsHolder->updateProtocol.body->id();
                        // if (auto validFocusedFood = focusedFood.lock()) {
                        //     std::cout << "\tfocused on: " << validFocusedFood->id() << "\n";
                        // }
                        break;
                    }
                    indexOfObject--;
                }
            }
        
        
            angle += deltaAngle(gen);
            angle = std::fmod(angle + M_PI, 2 * M_PI) - M_PI;
            protocolsHolder->updateProtocolResponce.actionMove(Vec2<float>(std::cos(angle), std::sin(angle)), 1.0f);
            
        }
    }

    void kill() override
    { // User defined brain class must have override kill function that takes 0 arguments
        population--;
        death++;
        printStats();
    }
};

int AggressiveMultiplierBotBrain::population = 0;
int AggressiveMultiplierBotBrain::death = 0;
unsigned long AggressiveMultiplierBotBrain::b_id = 0;

REGISTER_BOT_CLASS(AggressiveMultiplierBotBrain);