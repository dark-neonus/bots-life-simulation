#pragma once

#include <memory>

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
    BotBrain()
    : protocolsHolder(std::make_shared<ProtocolsHolder>()) {}

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

    // /// @brief Transform users brain object to format for spawning
    // /// @tparam T Type of users brain to spawn
    // /// @return Users brain object compressed to std::shared_tr<BotBrain>
    // template <typename T>
    // static std::shared_ptr<BotBrain> getBrainForSpawn() {
    //     return std::dynamic_pointer_cast<BotBrain>(std::make_shared<T>());
    // }
};
