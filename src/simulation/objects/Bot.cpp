#include "Bot.h"

#include <cmath>
#include <tuple>
#include <string>

#include "objects/SimulationObject.h"
#include "objects/Food.h"
#include "objects/Tree.h"
#include "simulation.h"

#include "protocols/shadows/ShadowBotObject.h"
#include "protocols/ProtocolsHolder.h"
#include "protocols/brain/BotBrain.h"

BotObject::BotObject(std::shared_ptr<Simulation> simulation,
            Vec2<int> position,
            float health_,
            float food_,
            int see_distance_,
            float speed_,
            float damage_,
            float maxHealth,
            float maxFood)
    : health(health_, 0, maxHealth == -1.0f ? health_ : maxHealth),
        food(food_, 0, maxFood == -1.0f ? food_ : maxFood),
        see_distance(see_distance_), speed(speed_), damage(damage_),
        SimulationObject(simulation, position, convertCaloriesToRadius(food_), colorInt(0, 75, 150)),
        shadow(std::make_shared<ShadowBotObject>(id.get(),
                                                pos,
                                                getRadius(),
                                                health.get(),
                                                food.get(),
                                                see_distance,
                                                speed,
                                                damage,
                                                health.getMax(),
                                                food.getMax())),
        protocolsHolder(std::make_shared<ProtocolsHolder>())
    {
    // This sh*t dosnt work :(
    protocolsHolder->updateProtocol.body = shadow;
    }

void BotObject::update()
{
    food.decrease(0.1);
    if (food.get() == 0)
    {
        health.decrease(0.5);
    }
    packProtocol();
    brain->update(brain->protocolsHolder->updateProtocol, brain->protocolsHolder->updateProtocolResponce);
    parseProtocolResponce();
    if (health.get() == 0)
    {
        markForDeletion();
    } else if (health.get() < health.getMax() && food.get() >= food.getMax() / 2)
    {
        // Healing logic
        health.increase(0.1);
        food.decrease(0.2);
    }

    underAttack = false;
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
    case BotAction::Spawn:
        actionSpawnBot(
            protocolsHolder->updateProtocolResponce.spawnArgs.brain,
            protocolsHolder->updateProtocolResponce.spawnArgs.evolutionPoints
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
        std::shared_ptr<BotObject> nearestBot;
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
                            auto nearestBotUnchecked = std::static_pointer_cast<BotObject>(validObj);
                            if (attackOwnKind || nearestBotUnchecked->brain->populationName != brain->populationName) {
                                minDistance = pos.sqrDistanceTo(validObj->pos);
                                nearestBot = nearestBotUnchecked;
                            }
                        }
                    }
                    else if (validObj->id.get() == targetID)
                    {
                        // Find object with given ID
                        if (validObj->type() == SimulationObjectType::BotObject)
                        {
                            auto nearestBotUnchecked = std::static_pointer_cast<BotObject>(validObj);
                            if (attackOwnKind || nearestBotUnchecked->brain->populationName != brain->populationName) {
                                minDistance = pos.sqrDistanceTo(validObj->pos);
                                nearestBot = nearestBotUnchecked;
                            }
                        }
                        // Clear chunksToCheck since found desired object
                        while (!chunksToCheck.empty()) { chunksToCheck.pop(); }
                        break;
                    }
                }
            }
            if (!chunksToCheck.empty()) {
                chunksToCheck.pop();
            }
        }
        // Small penalty for using actionAttack to prevent spam
        food.decrease(0.1);
        if (nearestBot && minDistance <= (getRadius() + nearestBot->getRadius()) * (getRadius() + nearestBot->getRadius()))
        {
            rawAttack(nearestBot);
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
    targetBot->underAttack = true;
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
                            minDistance = pos.sqrDistanceTo(nearestFood->pos);
                        }
                        // Clear chunksToCheck since found desired object
                        while (!chunksToCheck.empty()) { chunksToCheck.pop(); }
                        break;
                    }
                }
            }
            if (!chunksToCheck.empty()) {
                chunksToCheck.pop();
            }
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

void BotObject::actionSpawnBot(std::shared_ptr<BotBrain> brain, int evolutionPoints) {
    if (auto validSimulation = simulation.lock()) {
        if (evolutionPoints == -1) {
            evolutionPoints = validSimulation->settings->evolutionPointsSettings.amountOfPoints;
        }
        float minusHealth = std::max(0.0f, evolutionPoints - food.get());
        food.decrease(evolutionPoints);
        health.decrease(minusHealth);
        // If you try to spawn bot with evolution points more than mother can produce,
        // it will kill mother bot and wont create child bot
        if (health.get() > 0) {
            validSimulation->addBotToBorn(
                std::tuple<std::shared_ptr<BotBrain>, Vec2<float>, int>(
                    brain, pos, evolutionPoints
                )
            );
        }
    }
    else {
        throw std::runtime_error("Invalid simulation pointer of BotObject!");
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
    brain->kill(brain->protocolsHolder->killProtocol, brain->protocolsHolder->killProtocolResponce);
}

void BotObject::packProtocol()
{
    // Pack shadow object
    shadow->_health = health.get();
    shadow->_food = food.get();
    shadow->_seeDistance = getSeeDistance();
    shadow->_speed = speed;
    shadow->_damage = damage;
    shadow->_pos = pos;
    shadow->_underAttack = underAttack;

    if (!protocolsHolder->updateProtocol.body) {
        protocolsHolder->updateProtocol.body = std::const_pointer_cast<const ShadowBotObject>(shadow);
    }

    // Pack visible objects

    const int radius = getSeeDistance();
    const int sqrSeeDistance = radius * radius;
    auto chunksInVision = getChunksInRadius(pos, radius);

    protocolsHolder->updateProtocol.visibleObjects.clear();
    protocolsHolder->updateProtocol.visibleFood.clear();
    protocolsHolder->updateProtocol.visibleTree.clear();
    protocolsHolder->updateProtocol.visibleFriends.clear();
    protocolsHolder->updateProtocol.visibleEnemies.clear();

    protocolsHolder->updateProtocol.distanceToNearestBot = -1.0f;
    protocolsHolder->updateProtocol.distanceToNearestFriend = -1.0f;
    protocolsHolder->updateProtocol.distanceToNearestEnemy = -1.0f;
    protocolsHolder->updateProtocol.distanceToNearestFood = -1.0f;
    protocolsHolder->updateProtocol.distanceToNearestTree = -1.0f;

    float sqrDistanceToObj;

    protocolsHolder->updateProtocol.nearestBot = nullptr;
    protocolsHolder->updateProtocol.nearestFriend = nullptr;
    protocolsHolder->updateProtocol.nearestEnemy = nullptr;
    protocolsHolder->updateProtocol.nearestFood = nullptr;
    protocolsHolder->updateProtocol.nearestTree = nullptr;

    std::shared_ptr<const ShadowFoodObject> foodObj;
    std::shared_ptr<const ShadowTreeObject> treeObj;
    std::shared_ptr<const ShadowBotObject> botObj;

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
                        foodObj = std::dynamic_pointer_cast<FoodObject>(validChunkObject)->getShadow();
                        protocolsHolder->updateProtocol.visibleObjects.insert(foodObj);
                        protocolsHolder->updateProtocol.visibleFood.insert(foodObj);

                        break;
                    case SimulationObjectType::TreeObject:
                        if (protocolsHolder->updateProtocol.distanceToNearestTree == -1.0f ||
                            sqrDistanceToObj < protocolsHolder->updateProtocol.distanceToNearestTree)
                        {
                            protocolsHolder->updateProtocol.distanceToNearestTree = sqrDistanceToObj;
                            protocolsHolder->updateProtocol.nearestTree = std::dynamic_pointer_cast<TreeObject>(validChunkObject)->getShadow();
                        }
                        treeObj = std::dynamic_pointer_cast<TreeObject>(validChunkObject)->getShadow();
                        protocolsHolder->updateProtocol.visibleObjects.insert(treeObj);
                        protocolsHolder->updateProtocol.visibleTree.insert(treeObj);
                        break;
                    case SimulationObjectType::BotObject:
                        botObj = std::dynamic_pointer_cast<BotObject>(validChunkObject)->getShadow();
                        if (botObj->populationName() == protocolsHolder->updateProtocol.body->populationName()) {
                            // Bot is from the same population (Friend)
                            if (protocolsHolder->updateProtocol.distanceToNearestFriend == -1.0f ||
                                sqrDistanceToObj < protocolsHolder->updateProtocol.distanceToNearestFriend) {
                                    protocolsHolder->updateProtocol.distanceToNearestFriend = sqrDistanceToObj;
                                    protocolsHolder->updateProtocol.nearestFriend = botObj;
                                }
                            protocolsHolder->updateProtocol.visibleFriends.insert(botObj);
                        }
                        else {
                            // Bot is from different population (Enemy)
                            if (protocolsHolder->updateProtocol.distanceToNearestEnemy == -1.0f ||
                                sqrDistanceToObj < protocolsHolder->updateProtocol.distanceToNearestEnemy) {
                                    protocolsHolder->updateProtocol.distanceToNearestEnemy = sqrDistanceToObj;
                                    protocolsHolder->updateProtocol.nearestEnemy = botObj;
                                }
                            protocolsHolder->updateProtocol.visibleEnemies.insert(botObj);
                        }
                        protocolsHolder->updateProtocol.visibleObjects.insert(botObj);
                        protocolsHolder->updateProtocol.visibleBots.insert(botObj);
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
    if (protocolsHolder->updateProtocol.distanceToNearestFriend != -1.0f) {
        protocolsHolder->updateProtocol.distanceToNearestFriend = sqrtf(protocolsHolder->updateProtocol.distanceToNearestFriend);
        // Set nearest bot object
        protocolsHolder->updateProtocol.nearestBot = protocolsHolder->updateProtocol.nearestFriend;
        protocolsHolder->updateProtocol.distanceToNearestBot = protocolsHolder->updateProtocol.distanceToNearestFriend;
    }
    if (protocolsHolder->updateProtocol.distanceToNearestEnemy != -1.0f) {
        protocolsHolder->updateProtocol.distanceToNearestEnemy = sqrtf(protocolsHolder->updateProtocol.distanceToNearestEnemy);
        // Re-set nearest bot if enemy is more near than friend or there are no friends
        if (protocolsHolder->updateProtocol.distanceToNearestFriend == -1 ||
                protocolsHolder->updateProtocol.distanceToNearestFriend <= protocolsHolder->updateProtocol.distanceToNearestEnemy) {
            protocolsHolder->updateProtocol.nearestBot = protocolsHolder->updateProtocol.nearestEnemy;
            protocolsHolder->updateProtocol.distanceToNearestBot = protocolsHolder->updateProtocol.distanceToNearestEnemy;
        }
    }
}

void BotObject::setBrainObject(std::shared_ptr<BotBrain> brain_)
{
    brain = brain_;
    protocolsHolder = brain->protocolsHolder;
    shadow->_populationName = brain->populationName;
}

bool BotObject::isUnderAttack() const {
    return underAttack;
}

void BotObject::displayInfo()
{
    // Call parent class displayInfo to show basic information
    SimulationObject::displayInfo();

    // Add custom behavior for this class
    ImGui::SeparatorText("Bot Object");
    ImGui::Text("Population Name: %s", brain->populationName.c_str());
    // ImGui::Text("Shadow population Name: %s", shadow->populationName().c_str());
    // ImGui::InputFloat("Custom Value", &customValue, 1.0f, 1.0f, "%.2f");
    ImGui::SliderFloat("Health", health.valuePointer(), health.getMin(), health.getMax(), "%.1f");
    ImGui::SliderFloat("Food", food.valuePointer(), food.getMin(), food.getMax(), "%.1f calories");
    // TODO: Here in future we need to specify min and max value for see distance
    if (auto simulationValid = simulation.lock())
    {
        ImGui::SliderInt("See distance", &see_distance, 1, simulationValid->maxSeeDistance);
    }
    // TODO: Here in future we need to specify min and max value for speed
    ImGui::SliderFloat("Speed", &speed, 0.1f, 10.0f, "%.2f");
    // TODO: Here in future we need to specify min and max value for damage
    ImGui::SliderFloat("Damage", &damage, 0.0f, 20.0f, "%.2f");
    ImGui::Checkbox("Debug drawing", &debug_drawing);
}