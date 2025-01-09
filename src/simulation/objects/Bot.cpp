#include "Bot.h"
#include "objects/SimulationObject.h"
#include "objects/Food.h"
#include "simulation.h"

void BotObject::actionSpawnBot(float newHealth, float newFood,
    int newSeeDistance, float newSpeed, float newDamage) {

    if (newHealth < 0) newHealth = health.getMax();
    if (newFood < 0) newFood = food.getMax();
    if (newSeeDistance < 0) newSeeDistance = see_distance;
    if (newSpeed < 0) newSpeed = speed;
    if (newDamage < 0) newDamage = damage;

    const float spawnCost = food.getMax() * 0.3f;

    if (food.get() < spawnCost) {
        return;
    }

    if (auto validSimulation = simulation.lock()) {
        Vec2<int> newBotSpawnPos = Vec2<int>(
            pos.x - (rand() % 100 - 50),
            pos.y - (rand() % 100 - 50)
        );

        // Makes position of spawn valid 
        auto clamp = [](int value, int minVal, int maxVal) -> int {
            if (value < minVal) return minVal;
            if (value > maxVal) return maxVal;
            return value;
            };

        newBotSpawnPos.x = clamp(newBotSpawnPos.x, 20, validSimulation->chunkManager->mapWidth - 20);
        newBotSpawnPos.y = clamp(newBotSpawnPos.y, 20, validSimulation->chunkManager->mapHeight - 20);

        auto newBot = std::make_shared<BotObject>(
            validSimulation,
            newBotSpawnPos,
            health.getMax(),
            food.getMax(),
            see_distance,
            speed,
            damage
        );

        validSimulation->addObject(SimulationObjectType::BotObject, newBot);
        food.decrease(spawnCost);
    }
}

void BotObject::actionSuicide() {
    markForDeletion();
}

void BotObject::onDestroy() {
        if (auto validSimulation = simulation.lock()) {
            float calories = health.getMax() * 0.3 + food.get() * 0.7;
            // validSimulation->addObject(SimulationObjectType::FoodObject, validSimulation, pos, colorInt(100, 0, 0), calories, calories, 0, 5.0f, true);
            validSimulation->addObject(SimulationObjectType::FoodObject,
                            std::make_shared<FoodObject>(
                                validSimulation,
                                pos,
                                colorInt(100, 0, 0),
                                calories,
                                calories,
                                0,
                                5.0f,
                                true));
        }
    }