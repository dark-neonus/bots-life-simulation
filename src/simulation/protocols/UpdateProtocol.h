#pragma once

#include <unordered_set>
#include <memory>
#include <string>

#include "shadows/ShadowSimulationObject.h"
#include "shadows/ShadowFoodObject.h"
#include "shadows/ShadowTreeObject.h"
#include "shadows/ShadowBotObject.h"

using shadowObjectSet = std::unordered_set<std::shared_ptr<ShadowSimulationObject>,
                                           std::hash<std::shared_ptr<ShadowSimulationObject>>,
                                           std::equal_to<std::shared_ptr<ShadowSimulationObject>>>;

enum BotAction
{
    DoNothing,     ///< Perform no action
    Move,          ///< Move in a specific direction
    GoTo,          ///< Move to a target position
    EatNearest,    ///< Eat the nearest consumable object
    EatByID,       ///< Eat a specific object by ID
    AttackNearest, ///< Attack the nearest target
    AttackByID,    ///< Attack a specific target by ID
    Spawn,         ///< Spawn a new bot
    SpawnSelfCopy, ///< Spawn a copy of itself
    Suicide        ///< Self-destruct
};

struct UpdateProtocolResponce
{
    UpdateProtocolResponce() {}

    /// @brief Indicate which action to perform and from which member get information
    BotAction actionType;

    struct DoNothingInfo
    {
    };
    DoNothingInfo doNothingArgs;
    /// @brief Set action to DoNothing and set its arguments
    void actionDoNothing()
    {
        actionType = BotAction::DoNothing;
    };

    struct MoveInfo
    {
        Vec2<float> direction; ///< Direction vector for movement
        float speedMultiplier; ///< Length of movement vector relative to speed, must be in range [0.0, 1.0]
    };
    MoveInfo moveArgs;
    /// @brief Set action to Move and set its arguments
    /// @param direction Indicate in which direction bot will go
    /// @param speedMultiplier Indicate length of vector relative to speed parameter. Must be in range [0.0, 1.0]
    void actionMove(
        Vec2<float> direction,
        float speedMultiplier)
    {
        actionType = BotAction::Move;
        moveArgs.direction = direction;
        moveArgs.speedMultiplier = speedMultiplier;
    }

    struct GoToInfo
    {
        Vec2<float> targetPosition; ///< Target position to move towards
    };
    GoToInfo goToArgs;
    /// @brief Set action to GoTo and set its arguments
    /// @param targetPosition Target position to move to
    void actionGoTo(Vec2<float> targetPosition)
    {
        actionType = BotAction::GoTo;
        goToArgs.targetPosition = targetPosition;
    }

    struct EatNearestInfo
    {
    };
    EatNearestInfo eatNearestArgs;
    /// @brief Set action to EatNearest with no additional arguments
    void actionEatNearest()
    {
        actionType = BotAction::EatNearest;
    }

    struct EatByIDInfo
    {
        unsigned long objectID; ///< ID of the object to eat
    };
    EatByIDInfo eatByIDArgs;
    /// @brief Set action to EatByID and set its arguments
    /// @param objectID ID of the object to eat
    void actionEatByID(unsigned long objectID)
    {
        actionType = BotAction::EatByID;
        eatByIDArgs.objectID = objectID;
    }

    struct AttackNearestInfo
    {
    };
    AttackNearestInfo attackNearestArgs;
    /// @brief Set action to AttackNearest with no additional arguments
    void actionAttackNearest()
    {
        actionType = BotAction::AttackNearest;
    }

    struct AttackByIDInfo
    {
        unsigned long targetID; ///< ID of the target to attack
    };
    AttackByIDInfo attackByIDArgs;
    /// @brief Set action to AttackByID and set its arguments
    /// @param targetID ID of the target to attack
    void actionAttackByID(unsigned long targetID)
    {
        actionType = BotAction::AttackByID;
        attackByIDArgs.targetID = targetID;
    }

    // struct SpawnInfo
    // {
    //     int spawnType;               ///< Type of object to spawn
    // };
    // SpawnInfo spawnArgs;
    // /// @brief Set action to Spawn and set its arguments
    // /// @param spawnType Type of object to spawn
    // void actionSpawn(int spawnType)
    // {
    //     actionType = BotAction::Spawn;
    //     spawnArgs.spawnType = spawnType;
    // }

    struct SpawnSelfCopyInfo
    {
    };
    SpawnSelfCopyInfo spawnSelfCopyArgs;
    /// @brief Set action to SpawnSelfCopy with no additional arguments
    void actionSpawnSelfCopy()
    {
        actionType = BotAction::SpawnSelfCopy;
    }

    struct SuicideInfo
    {
    };
    SuicideInfo suicideArgs;
    /// @brief Set action to Suicide with no additional arguments
    void actionSuicide()
    {
        actionType = BotAction::Suicide;
    }
};

struct UpdateProtocol
{
    UpdateProtocol() {}
    
    /// @brief Type of action performed by bot on the last update
    BotAction lastActionType;
    /// @brief Indicate weather performing last action was successfull or not
    bool lastActionSuccess;
    /// @brief Message that can contain some information about last performed action
    std::string lastActionMessage;

    /// @brief ShadowBotObject that represnt body of current bot
    std::shared_ptr<ShadowBotObject> body;
    /*
     * Unordered set of shared_ptr<ShadowBotObject> representing
     * all objects that are in vision radius of bot;
     */
    shadowObjectSet visibleObjects;

    /*
     * shared_ptr<ShadowBotObject> representing nearest bot
     * in the vision. Can be null_ptr if there is no bots in the vision.
     */
    std::shared_ptr<ShadowBotObject> nearestBot;
    /*
     * Distance to the nearest bot in the vision.
     * If there is no bots in the vision, its value is -1.0f.
     */
    float distanceToNearestBot;

    /*
     * shared_ptr<ShadowBotObject> representing nearest enemy(bot of different type)
     * in the vision. Can be null_ptr if there is no enemies in the vision.
     */
    std::shared_ptr<ShadowBotObject> nearestEnemy;
    /*
     * Distance to the nearest enemy(bot of different type) in the vision.
     * If there is no enemies in the vision, its value is -1.0f.
     */
    float distanceToNearestEnemy;

    /*
     * shared_ptr<ShadowFoodObject> representing nearest food
     * in the vision. Can be null_ptr if there is no food in the vision.
     */
    std::shared_ptr<ShadowFoodObject> nearestFood;
    /*
     * Distance to the nearest food in the vision.
     * If there is no food in the vision, its value is -1.0f.
     */
    float distanceToNearestFood;

    /*
     * shared_ptr<ShadowTreeObject> representing nearest tree
     * in the vision. Can be null_ptr if there is no tree in the vision.
     */
    std::shared_ptr<ShadowTreeObject> nearestTree;
    /*
     * Distance to the nearest tree in the vision.
     * If there is no tree in the vision, its value is -1.0f.
     */
    float distanceToNearestTree;
};
