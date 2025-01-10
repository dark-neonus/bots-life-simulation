#pragma once

#include "protocols/ProtocolsHolder.h"
#include "objects/Bot.h"

class BotBrain
{
private:
    friend BotObject;
protected:
    /// @brief Holder for all protocols of communication between brain and simulation
    std::shared_ptr<ProtocolsHolder> protocolsHolder;
public:
    BotBrain() {}

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
};
