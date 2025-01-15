#pragma once

#include <memory>
#include <string>

#include "protocols/ProtocolsHolder.h"
#include "objects/Bot.h"
#include "simulation.h"
class BotBrain
{
private:
    friend BotObject;
    friend Simulation;
protected:
    /// @brief Holder for all protocols of communication between brain and simulation
    std::shared_ptr<ProtocolsHolder> protocolsHolder;
public:
    const std::string populationName;

    BotBrain(std::string populationName_="DefaultType")
    : populationName(populationName_),
      protocolsHolder(std::make_shared<ProtocolsHolder>()) {
      }

    /*
     * Function that will be called on creation of the bot.
     * It should read protocolsHolder->InitProtocol and modify protocolsHolder->InitProtocolResponce
     * setting all parameters needed to create a new BotObject.
     */
    virtual void init() {}
    /*
     * Function that will be called on each frame of the simulation.
     * It should read protocolsHolder->UpdateProtocol and modify protocolsHolder->UpdateProtocolResponce
     * telling the BotObject what to do and howto do it.
     */
    virtual void update() {}
    /*
     * Function that will be called on the died of the bot.
     * It should read protocolsHolder->KillProtocol and modify protocolsHolder->KillProtocolResponce
     * doing all thing needed to delete/kill bot (e.g. dealocate allocated memory).
     */
    virtual void kill() {}

    /*
     * Check if given object can be reached
     * (Compare distance between them to sum of their radiuses)
     */
    bool canReach(std::shared_ptr<const ShadowSimulationObject> obj) {
        return protocolsHolder->updateProtocol.body->pos().sqrDistanceTo(obj->pos()) <= 
            (protocolsHolder->updateProtocol.body->radius() + obj->radius()) * (protocolsHolder->updateProtocol.body->radius() + obj->radius());
    }

    /*
     * Check if given bot is from the same population
     */
    bool isSamePopulation(std::shared_ptr<const ShadowBotObject> obj) {
        return populationName == obj->populationName();
    }

    /*
     * Check if there are any friends in vision
     * (Check if protocolsHolder->updateProtocol.distanceToNearestFriend != -1.0f)
     */
    bool seeFriend() { return protocolsHolder->updateProtocol.distanceToNearestFriend != -1.0f; }

    /*
     * Check if there are any enemy in vision
     * (Check if protocolsHolder->updateProtocol.distanceToNearestEnemy != -1.0f)
     */
    bool seeEnemy() { return protocolsHolder->updateProtocol.distanceToNearestEnemy != -1.0f; }


    /*
     * Check if there are any food in vision
     * (Check if protocolsHolder->updateProtocol.distanceToNearestFoood != -1.0f)
     */
    bool seeFood() { return protocolsHolder->updateProtocol.distanceToNearestFood != -1.0f; }
};
