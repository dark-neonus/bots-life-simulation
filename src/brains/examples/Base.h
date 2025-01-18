#pragma once

// Your includes here

// Include requried files for brain creation
#include "protocols/brain/BotBrain.h"

// You may need Vec2 class for coordinates
#include "utilities/Vec2.h"

/*
 * To create bots brain class, define class
 * that publically inherit from BotBrain
 * (You can check BotBrain for deeper understanding of brains structure)
 */
class YourBotClassName : public BotBrain
{
private:
    /*
     * Optional variable that indicate amount
     * of current live bots of this population in simulation
     */
    static int population;
    /*
     * Optional variable that indicate amount
     * of death in this population during simulation
     */
    static int death;
    /*
     * Optional variable that indicate amount
     * of spawned bots of this population during simulation
     */
    static unsigned long b_id;
    /*
     * Function that output information about population when something changed
     */
    void printStats() {
        std::cout << "<" << populationName;
        std::cout << "> |- population: " << population;
        std::cout << " | death: " << death;
        std::cout << " | b_id: " << b_id << " |\n";
    }
public:

    /*
     * Here is constructor that must call BotBrain()
     * constructor in init list and pass unique population name to it
     */
    YourBotClassName() 
    : BotBrain("AgressiveMultiplierLegion")
    {
    }

    /*
     * @brief Function that will be called when bot is born.
     * Here you should set bots color, distribute evolution points
     * to each bots property. Also you can do here anything
     * you need to init bots brain.
     * Name and parameters must remain the same, as well as override flag
     * 
     * @param data Shortcut for protocolsHolder->initProtocol
     * that contains information about simulation and bot start stats
     * (e.g. amount of evolution points given to bot)
     * Check InitProtocol class for more information.
     * 
     * @param responce Shortcut for protocolsHolder->initProtocloResponce
     * that contain values that you need to set, which will indicate
     * bots stats and properties
     * (e.g. color, evolution points given to health parameter)
     * Check InitProtocolClass for more information.
     */
    void init(InitProtocol& data, InitProtocolResponce& responce) override 
    {
        /// Setting bots color here
        responce.r = 210;
        responce.g = 130;
        responce.b = 30;

        /// Distributing evolution points here
        responce.healthPoints = int(0.05 * data.evolutionPoints);
        responce.foodPoints = int(0.25 * data.evolutionPoints);
        responce.visionPoints = int(0.37 * data.evolutionPoints);
        responce.speedPoints = int(0.25 * data.evolutionPoints);
        responce.attackPoints = int(0.07 * data.evolutionPoints);

        // Optional things. Increase population and b_id counters. Output stats 
        population++;
        b_id++;
        printStats();
    }

    /*
     * @brief Function that is being called every frame.
     * Here you should do analize situation and call
     * specific action that will be performed by bot.
     * All actions and their parameters you can check in UpdateProtocolResponce class.
     * Name and parameters must remain the same, as well as override flag
     * 
     * @param data Shortcut for protocolsHolder->updateProtocol
     * that contains information about bots body and what he see.
     * (e.g. amount of evolution points given to bot)
     * Check UpdateProtocol class for more information.
     * 
     * @param responce Shortcut for protocolsHolder->updateProtocloResponce
     * mainly purpose of which is to hold action and its arguments.
     * (e.g. actionMove and moving direction)
     * Check UpdateProtocolResponce class for more information.
     */
    void update(UpdateProtocol& data, UpdateProtocolResponce& responce) override
    {
        // Here is basic and stupid bot behavior for example

        // If there are enemies in vision radius
        if (seeEnemy()) {
            // If can reach nearest enemy
            if (canReach(data.nearestEnemy)) {
                // Attack nearest enemy
                responce.actionAttackByID(false, data.nearestEnemy->id());
            }
            // If cant reach nearest enemy
            else {
                // Go to nearest enemy
                responce.actionGoTo(data.nearestEnemy->pos());
            }
            // Use return to avoid further action overriding
            return;
        }
        
        // If there are food in vision radius
        if (seeFood()) {
            // If can reach nearest food
            if (canReach(data.nearestFood)) {
                // Eat nearest food
                responce.actionEatByID(data.nearestFood->id());
            }
            // If cant reach nearest food
            else {
                // Go to nearest food
                responce.actionGoTo(data.nearestFood->pos());
            }
            // Use return to avoid further action overriding
            return;
        }

        // If there are no enemies and food in sigh, move somewhere 
        responce.actionMove(Vec2<float>(2.0f, -1.0f), 0.9f);
    }

    /*
     * @brief Function that will be called when bot die.
     * In most cases, this function will be empty,
     * or will contain some counters update.
     * If you allocated some memory in your bots brain,
     * we kindly ask and recommend to deallocate it here.
     * This will prevent memory leaks.
     * Name and parameters must remain the same, as well as override flag
     * 
     * @param data Shortcut for protocolsHolder->killProtocol
     * that is empty for now
     * Check KillProtocol class for more information.
     * 
     * @param responce Shortcut for protocolsHolder->killProtocloResponce
     * that contain only success flag for now.
     * If your kill function went ok, set success to true, please.
     * Check KillProtocolClass for more information.
     */
    void kill(KillProtocol& data, KillProtocolResponce& responce) override
    {
        // Optional things. Decrease population and increase death counters. Output stats
        population--;
        death++;
        printStats();
        // Set success of kill process to true
        responce.success = true;
    }
};

// Init values of static brain members
int YourBotClassName::population = 0;
int YourBotClassName::death = 0;
unsigned long YourBotClassName::b_id = 0;