#pragma once

#include <random>
#include <cmath>
#include <memory>

#include "protocols/brain/BotBrain.h"


class AggressiveMultiplierBotBrain2 : public BotBrain
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
    AggressiveMultiplierBotBrain2() 
    : BotBrain("AgressiveMultiplierLegion2")
    {
    }

    void init(InitProtocol& data, InitProtocolResponce& responce) override 
    { // User defined brain class must have override init function that takes 0 arguments
        responce.r = 180;
        responce.g = 30;
        responce.b = 210;

        responce.healthPoints = int(0.05 * data.evolutionPoints);
        responce.foodPoints = int(0.25 * data.evolutionPoints);
        responce.visionPoints = int(0.37 * data.evolutionPoints);
        responce.speedPoints = int(0.25 * data.evolutionPoints);
        responce.attackPoints = int(0.07 * data.evolutionPoints);

        angle = circleAngle(gen);
        population++;
        b_id++;
        gen = std::mt19937(static_cast<std::mt19937::result_type>(rd() + b_id));
        printStats();
    }

    int focusTime = 0;
    int bornAmount = 0;

    std::weak_ptr<const ShadowSimulationObject> focusedFood;

    void update(UpdateProtocol& data, UpdateProtocolResponce& responce) override
    { // User defined brain class must have override update function that takes 0 arguments
        // std::cout << "process_bot_: " << protocolsHolder->updateProtocol.body->id() << "\n";
        // if (bornAmount >= 2 && population > 120 || bornAmount >= 1 && population > 500) {
        //     protocolsHolder->updateProtocolResponce.actionSuicide();
        //     return;
        // }
        if (seeEnemy() &&
            // protocolsHolder->updateProtocol.nearestEnemy->speed() + 2 < protocolsHolder->updateProtocol.body->speed() &&
            data.body->food() >= 0.6f * data.body->maxFood()) {
            if (canReach(data.nearestEnemy)) {
                responce.actionAttackByID(false, data.nearestEnemy->id());
            }
            else {
                responce.actionGoTo(data.nearestEnemy->pos());
            }
            return;
        }
        if (auto validFocusedFood = focusedFood.lock()) {
            if (canReach(validFocusedFood)) {
                if (data.body->food() >= 0.9f * data.body->maxFood() &&
                    data.body->health() == data.body->maxHealth()) {
                    responce.actionSpawn(
                        std::dynamic_pointer_cast<BotBrain>(
                            std::make_shared<AggressiveMultiplierBotBrain2>()
                        ),
                        protocolsHolder->initProtocol.evolutionPoints + 5
                    );
                    bornAmount++;
                }
                else {
                    // std::cout << "try to eat: " << validFocusedFood->id() << "\n";
                    // std::cout << "myPos: " << protocolsHolder->updateProtocol.body->pos().text() << "targetPos: " << validFocusedFood->pos() << "\n";
                    responce.actionEatByID(validFocusedFood->id());
                }
            }
            else {
                responce.actionGoTo(validFocusedFood->pos());
            }
        }
        else {
            int indexOfObject = randomDirChance(gen);
            for (auto& obj : data.visibleObjects) {
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
            responce.actionMove(Vec2<float>(std::cos(angle), std::sin(angle)), 1.0f);
            
        }
    }

    void kill(KillProtocol& data, KillProtocolResponce& responce) override
    { // User defined brain class must have override kill function that takes 0 arguments
        population--;
        death++;
        printStats();
        responce.success = true;
    }
};

int AggressiveMultiplierBotBrain2::population = 0;
int AggressiveMultiplierBotBrain2::death = 0;
unsigned long AggressiveMultiplierBotBrain2::b_id = 0;