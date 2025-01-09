#include "Bot.h"
#include "objects/SimulationObject.h"
#include "objects/Food.h"
#include "simulation.h"

void BotObject::actionMove(Vec2<float> direction, float speedMultyplier) {
        food.decrease(0.1);
        direction = direction.normalize();
        speedMultyplier = std::clamp<float>(speedMultyplier, 0.0f, 1.0f);
        if (auto validSimulation = simulation.lock()) {
            pos = Vec2<float>(
                std::clamp(pos.x + direction.x * speed * speedMultyplier,
                            0.0f, validSimulation->chunkManager->mapWidth),
                std::clamp(pos.y + direction.y * speed * speedMultyplier,
                            0.0f, validSimulation->chunkManager->mapHeight)
            );
            if (auto validChunk = chunk.lock()) {
                if (!validChunk->isPosInsideChunk(pos)) {
                    validChunk->moveToChunk(shared_from_this(), validSimulation->chunkManager->whatChunkHere(pos));
                }
            }
            else {
                throw std::runtime_error("Invalid chunk pointer of BotObject!");
            }
        }
        else {
            throw std::runtime_error("Invalid simulation pointer of BotObject!");
        }
    }

void BotObject::actionAttack(bool attackOwnKind, unsigned long targetID) {
    // When each users program will have own type id, add logic for attackOwnKind
    std::queue<std::shared_ptr<Chunk>> chunksToCheck;
    if (auto validChunk = chunk.lock()) {
        std::shared_ptr<Chunk> neighborChunk;
        if (!validChunk->isPosInsideChunk(pos - getRadius()) || !validChunk->isPosInsideChunk(pos + getRadius())) {
            if (auto validSimulation = simulation.lock()) {
                for (int y = -1; y < 2; y++) {
                    for (int x = -1; x < 2; x++) {
                        neighborChunk = validSimulation->chunkManager->getChunk(validChunk->xIndex + x, validChunk->yIndex + y);
                        if (neighborChunk) {
                            chunksToCheck.push(neighborChunk);
                        }
                    }
                }
            }
        } else {
            chunksToCheck.push(validChunk);
        }
        float minDistance = validChunk->chunkSize * 10;
        std::shared_ptr<SimulationObject> nearestBot;
        while (!chunksToCheck.empty()) {
            neighborChunk = chunksToCheck.front();
            for (auto obj : neighborChunk->objects) {
                if (auto validObj = obj.lock()) {
                    if (validObj->id.get() != id.get() &&
                            (targetID == ULONG_MAX || validObj->id.get() == targetID) &&
                            validObj->type() == SimulationObjectType::BotObject &&
                            pos.sqrDistanceTo(validObj->pos) < minDistance) {
                        minDistance = pos.sqrDistanceTo(validObj->pos);
                        nearestBot = validObj;
                    }
                }
            }
            chunksToCheck.pop();
        }
        // Small penalty for using actionAttack to prevent spam
        food.decrease(0.1);
        if (nearestBot && minDistance <= (getRadius() + nearestBot->getRadius()) * (getRadius() + nearestBot->getRadius())) {
            rawAttack(std::static_pointer_cast<BotObject>(nearestBot));
        }
    }
    else {
        throw std::runtime_error("Invalid chunk pointer of BotObject!");
    }
}

void BotObject::rawAttack(std::shared_ptr<BotObject> targetBot) {
    targetBot->health.decrease(damage);
    food.decrease(0.4);
}

void BotObject::actionSpawnBot() {
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