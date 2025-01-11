#include "Bot.h"

#include <cmath>

#include "objects/SimulationObject.h"
#include "objects/Food.h"
#include "objects/Tree.h"
#include "simulation.h"

#include "protocols/ProtocolsHolder.h"
#include "protocols/brain/BotBrain.h"

void BotObject::update()
{
    // Change values just to display debug values drawing
    if (debug_drawing)
    {
        // food.decrease(0.1);
        if (food.get() == 0)
        {
            health.decrease(0.5);
        }
    }
    if (health.get() == 0)
    {
        markForDeletion();
    }
    else if (health.get() < health.getMax() && food.get() >= food.getMax() / 2)
    {
        // Healing logic
        health.increase(0.1);
        food.decrease(0.2);
    }
    shadow->_health = health.get();
    shadow->_food = food.get();
    shadow->_seeDistance = getSeeDistance();
    shadow->_speed = speed;
    shadow->_damage = damage;
    shadow->_pos = pos;
    food.decrease(0.1);
    packProtocol();
    brain->update();
    parseProtocolResponce();
}

// enum BotAction
// {
//     DoNothing,     ///< Perform no action
//     Move,          ///< Move in a specific direction
//     GoTo,          ///< Move to a target position
//     EatNearest,    ///< Eat the nearest consumable object
//     EatByID,       ///< Eat a specific object by ID
//     AttackNearest, ///< Attack the nearest target
//     AttackByID,    ///< Attack a specific target by ID
//     Spawn,         ///< Spawn a new bot
//     SpawnSelfCopy, ///< Spawn a copy of itself
//     Suicide        ///< Self-destruct
// };

void BotObject::parseProtocolResponce() {
    switch (static_cast<int>(protocolsHolder->updateProtocolResponce.actionType))
    {
    case BotAction::DoNothing:
        break;
    case BotAction::Move:
        actionMove(
            protocolsHolder->updateProtocolResponce.moveArgs.direction,
            protocolsHolder->updateProtocolResponce.moveArgs.speedMultiplier
            );
        break;
    case BotAction::GoTo:
        actionGoTo(
            protocolsHolder->updateProtocolResponce.goToArgs.targetPosition
            );
        break;
    case BotAction::EatNearest:
        actionEat();
        break;
    case BotAction::EatByID:
        actionEat(
            protocolsHolder->updateProtocolResponce.eatByIDArgs.objectID
            );
        break;
    case BotAction::AttackNearest:
        actionAttack(
            protocolsHolder->updateProtocolResponce.attackNearestArgs.attackOwnKind
        );
        break;
    case BotAction::AttackByID:
        actionAttack(
            protocolsHolder->updateProtocolResponce.attackByIDArgs.attackOwnKind,
            protocolsHolder->updateProtocolResponce.attackByIDArgs.targetID
            );
        break;
    
    case BotAction::Suicide:
        actionSuicide();
        break;
    
    default:
        throw std::invalid_argument("Invalid action type!");
        break;
    }
}

void BotObject::actionMove(Vec2<float> direction, float speedMultyplier)
{
    direction = direction.normalize();
    speedMultyplier = std::clamp<float>(speedMultyplier, 0.0f, 1.0f);
    food.decrease(0.1 * speedMultyplier);
    if (auto validSimulation = simulation.lock())
    {
        pos = Vec2<float>(
            std::clamp(pos.x + direction.x * speed * speedMultyplier,
                       0.0f, validSimulation->chunkManager->mapWidth),
            std::clamp(pos.y + direction.y * speed * speedMultyplier,
                       0.0f, validSimulation->chunkManager->mapHeight));
        if (auto validChunk = chunk.lock())
        {
            if (!validChunk->isPosInsideChunk(pos))
            {
                validChunk->moveToChunk(shared_from_this(), validSimulation->chunkManager->whatChunkHere(pos));
            }
        }
        else
        {
            throw std::runtime_error("Invalid chunk pointer of BotObject!");
        }
    }
    else
    {
        throw std::runtime_error("Invalid simulation pointer of BotObject!");
    }
}

void BotObject::actionGoTo(Vec2<float> targetPos) {
    Vec2<float> delta = targetPos - pos;
    actionMove(delta, delta.length() / speed);
}

void BotObject::actionAttack(bool attackOwnKind, unsigned long targetID)
{
    // When each users program will have own type id, add logic for attackOwnKind
    std::queue<std::shared_ptr<Chunk>> chunksToCheck;
    if (auto validChunk = chunk.lock())
    {
        std::shared_ptr<Chunk> neighborChunk;
        if (!validChunk->isPosInsideChunk(pos - getRadius()) || !validChunk->isPosInsideChunk(pos + getRadius()))
        {
            if (auto validSimulation = simulation.lock())
            {
                for (int y = -1; y < 2; y++)
                {
                    for (int x = -1; x < 2; x++)
                    {
                        neighborChunk = validSimulation->chunkManager->getChunk(validChunk->xIndex + x, validChunk->yIndex + y);
                        if (neighborChunk)
                        {
                            chunksToCheck.push(neighborChunk);
                        }
                    }
                }
            }
        }
        else
        {
            chunksToCheck.push(validChunk);
        }
        float minDistance = validChunk->chunkSize * 10;
        std::shared_ptr<SimulationObject> nearestBot;
        while (!chunksToCheck.empty())
        {
            neighborChunk = chunksToCheck.front();
            for (auto obj : neighborChunk->objects)
            {
                if (auto validObj = obj.lock())
                {
                    if (targetID == ULONG_MAX)
                    {
                        // Find nearest if targetID wasnt specified
                        if (validObj->id.get() != id.get() &&
                            validObj->type() == SimulationObjectType::BotObject &&
                            pos.sqrDistanceTo(validObj->pos) < minDistance)
                        {
                            minDistance = pos.sqrDistanceTo(validObj->pos);
                            nearestBot = validObj;
                        }
                    }
                    else if (validObj->id.get() == targetID)
                    {
                        // Find object with given ID
                        if (validObj->type() == SimulationObjectType::BotObject)
                        {
                            nearestBot = validObj;
                        }
                        // Clear chunksToCheck since found desired object
                        chunksToCheck = std::queue<std::shared_ptr<Chunk>>();
                        break;
                    }
                }
            }
            chunksToCheck.pop();
        }
        // Small penalty for using actionAttack to prevent spam
        food.decrease(0.1);
        if (nearestBot && minDistance <= (getRadius() + nearestBot->getRadius()) * (getRadius() + nearestBot->getRadius()))
        {
            rawAttack(std::static_pointer_cast<BotObject>(nearestBot));
        }
    }
    else
    {
        throw std::runtime_error("Invalid chunk pointer of BotObject!");
    }
}

void BotObject::rawAttack(std::shared_ptr<BotObject> targetBot)
{
    targetBot->health.decrease(damage);
    food.decrease(0.4);
}

void BotObject::actionEat(unsigned long targetID)
{
    // When each users program will have own type id, add logic for attackOwnKind
    std::queue<std::shared_ptr<Chunk>> chunksToCheck;
    if (auto validChunk = chunk.lock())
    {
        std::shared_ptr<Chunk> neighborChunk;
        if (!validChunk->isPosInsideChunk(pos - getRadius()) || !validChunk->isPosInsideChunk(pos + getRadius()))
        {
            if (auto validSimulation = simulation.lock())
            {
                for (int y = -1; y < 2; y++)
                {
                    for (int x = -1; x < 2; x++)
                    {
                        neighborChunk = validSimulation->chunkManager->getChunk(validChunk->xIndex + x, validChunk->yIndex + y);
                        if (neighborChunk)
                        {
                            chunksToCheck.push(neighborChunk);
                        }
                    }
                }
            }
        }
        else
        {
            chunksToCheck.push(validChunk);
        }
        float minDistance = validChunk->chunkSize * 10;
        std::shared_ptr<SimulationObject> nearestFood;
        while (!chunksToCheck.empty())
        {
            neighborChunk = chunksToCheck.front();
            for (auto obj : neighborChunk->objects)
            {
                if (auto validObj = obj.lock())
                {
                    if (targetID == ULONG_MAX)
                    {
                        // Find nearest if targetID wasnt specified
                        if (validObj->id.get() != id.get() &&
                            validObj->type() == SimulationObjectType::FoodObject &&
                            pos.sqrDistanceTo(validObj->pos) < minDistance)
                        {
                            minDistance = pos.sqrDistanceTo(validObj->pos);
                            nearestFood = validObj;
                        }
                    }
                    else if (validObj->id.get() == targetID)
                    {
                        // Find object with given ID
                        if (validObj->type() == SimulationObjectType::FoodObject)
                        {
                            nearestFood = validObj;
                        }
                        // Clear chunksToCheck since found desired object
                        chunksToCheck = std::queue<std::shared_ptr<Chunk>>();
                        break;
                    }
                }
            }
            chunksToCheck.pop();
        }
        // Small penalty for using actionEat to prevent spam
        food.decrease(0.05);
        if (nearestFood && minDistance <= (getRadius() + nearestFood->getRadius()) * (getRadius() + nearestFood->getRadius()))
        {
            rawEat(std::static_pointer_cast<FoodObject>(nearestFood));
        }
    }
    else
    {
        throw std::runtime_error("Invalid chunk pointer of BotObject!");
    }
}

void BotObject::rawEat(std::shared_ptr<FoodObject> targetFood)
{
    // Calories needed for "chewing"
    food.decrease(0.1);
    float eatenCalories = targetFood->decreaseCalories(std::max(5.0f, food.getMax() / 20));
    food.increase(eatenCalories);
}

void BotObject::actionSpawnBot(float newHealth, float newFood,
                               int newSeeDistance, float newSpeed, float newDamage)
{

    if (newHealth < 0)
        newHealth = health.getMax();
    if (newFood < 0)
        newFood = food.getMax();
    if (newSeeDistance < 0)
        newSeeDistance = see_distance;
    if (newSpeed < 0)
        newSpeed = speed;
    if (newDamage < 0)
        newDamage = damage;

    const float spawnCost = food.getMax() * 0.3f;

    if (food.get() < spawnCost)
    {
        return;
    }

    if (auto validSimulation = simulation.lock())
    {
        Vec2<int> newBotSpawnPos = Vec2<int>(
            pos.x - (rand() % 100 - 50),
            pos.y - (rand() % 100 - 50));

        // Makes position of spawn valid
        auto clamp = [](int value, int minVal, int maxVal) -> int
        {
            if (value < minVal)
                return minVal;
            if (value > maxVal)
                return maxVal;
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
            damage);

        validSimulation->addObject(SimulationObjectType::BotObject, newBot);
        food.decrease(spawnCost);
    }
}

void BotObject::actionSuicide()
{
    markForDeletion();
}

void BotObject::onDestroy()
{
    if (auto validSimulation = simulation.lock())
    {
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
    brain->kill();
}

void BotObject::packProtocol()
{
    // It may be faster to use std::vector<std::shared_ptr<SimulationObject>>

    const int radius = getSeeDistance();
    const int sqrSeeDistance = radius * radius;
    auto chunksInVision = getChunksInRadius(pos, radius);

    protocolsHolder->updateProtocol.visibleObjects.clear();

    protocolsHolder->updateProtocol.distanceToNearestBot = -1.0f;
    protocolsHolder->updateProtocol.distanceToNearestEnemy = -1.0f;
    protocolsHolder->updateProtocol.distanceToNearestFood = -1.0f;
    protocolsHolder->updateProtocol.distanceToNearestTree = -1.0f;

    float sqrDistanceToObj;

    protocolsHolder->updateProtocol.nearestBot = nullptr;
    protocolsHolder->updateProtocol.nearestEnemy = nullptr;
    protocolsHolder->updateProtocol.nearestFood = nullptr;
    protocolsHolder->updateProtocol.nearestTree = nullptr;

    std::shared_ptr<FoodObject> foodObj;
    std::shared_ptr<TreeObject> treeObj;
    std::shared_ptr<BotObject> botObj;

    for (const auto &chunk : chunksInVision)
    {
        for (const auto &chunkObject : chunk->getObjects())
        {
            if (auto validChunkObject = chunkObject.lock())
            {
                sqrDistanceToObj = pos.sqrDistanceTo(validChunkObject->pos);
                if (sqrDistanceToObj < sqrSeeDistance && validChunkObject.get() != this)
                {
                    switch (validChunkObject->type())
                    {
                    case SimulationObjectType::BaseObject:
                        break;
                    case SimulationObjectType::FoodObject:
                        if (protocolsHolder->updateProtocol.distanceToNearestFood == -1.0f ||
                            sqrDistanceToObj < protocolsHolder->updateProtocol.distanceToNearestFood)
                        {
                            protocolsHolder->updateProtocol.distanceToNearestFood = sqrDistanceToObj;
                            protocolsHolder->updateProtocol.nearestFood = std::dynamic_pointer_cast<FoodObject>(validChunkObject)->getShadow();
                        }
                        protocolsHolder->updateProtocol.visibleObjects.insert(
                            std::dynamic_pointer_cast<FoodObject>(validChunkObject)->getShadow());
                        break;
                    case SimulationObjectType::TreeObject:
                        if (protocolsHolder->updateProtocol.distanceToNearestTree == -1.0f ||
                            sqrDistanceToObj < protocolsHolder->updateProtocol.distanceToNearestTree)
                        {
                            protocolsHolder->updateProtocol.distanceToNearestTree = sqrDistanceToObj;
                            protocolsHolder->updateProtocol.nearestTree = std::dynamic_pointer_cast<TreeObject>(validChunkObject)->getShadow();
                        }
                        protocolsHolder->updateProtocol.visibleObjects.insert(
                            std::dynamic_pointer_cast<TreeObject>(validChunkObject)->getShadow());
                        break;
                    case SimulationObjectType::BotObject:
                        if (protocolsHolder->updateProtocol.distanceToNearestBot == -1.0f ||
                            sqrDistanceToObj < protocolsHolder->updateProtocol.distanceToNearestBot)
                        {
                            protocolsHolder->updateProtocol.distanceToNearestBot = sqrDistanceToObj;
                            protocolsHolder->updateProtocol.nearestBot = std::dynamic_pointer_cast<BotObject>(validChunkObject)->getShadow();
                        }
                        protocolsHolder->updateProtocol.visibleObjects.insert(
                            std::dynamic_pointer_cast<BotObject>(validChunkObject)->getShadow());
                        break;
                    default:
                        throw std::runtime_error("Invalid simulation object type!");
                    }
                }
            }
        }
    }

    if (protocolsHolder->updateProtocol.distanceToNearestFood != -1.0f) {
        protocolsHolder->updateProtocol.distanceToNearestFood = sqrtf(protocolsHolder->updateProtocol.distanceToNearestFood);
    }
    if (protocolsHolder->updateProtocol.distanceToNearestTree != -1.0f) {
        protocolsHolder->updateProtocol.distanceToNearestTree = sqrtf(protocolsHolder->updateProtocol.distanceToNearestTree);
    }
    if (protocolsHolder->updateProtocol.distanceToNearestBot != -1.0f) {
        protocolsHolder->updateProtocol.distanceToNearestBot = sqrtf(protocolsHolder->updateProtocol.distanceToNearestBot);
    }
    if (protocolsHolder->updateProtocol.distanceToNearestEnemy != -1.0f) {
        protocolsHolder->updateProtocol.distanceToNearestEnemy = sqrtf(protocolsHolder->updateProtocol.distanceToNearestEnemy);
    }
}

void BotObject::setBrainObject(std::shared_ptr<BotBrain> brain_)
{
    brain = brain_;
    protocolsHolder = brain->protocolsHolder;
}