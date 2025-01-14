#pragma once

#include <memory>

#include "chunks.h"
#include "objects/SimulationObject.h"
#include "simulation.h"
#include "utilities/utilities.h"

#include "protocols/ProtocolsHolder.h"
#include "protocols/shadows/ShadowBotObject.h"

#include "protocols/brain/BotBrain.h"

class FoodObject;
class BotBrain;

#define DEFAULT_DEBUG_DRAWING false

using objectSet = std::unordered_set<std::weak_ptr<SimulationObject>,
                                     std::hash<std::weak_ptr<SimulationObject>>,
                                     std::equal_to<std::weak_ptr<SimulationObject>>>;

using shadowObjectSet = std::unordered_set<std::shared_ptr<const ShadowSimulationObject>,
                                           std::hash<std::shared_ptr<const ShadowSimulationObject>>,
                                           std::equal_to<std::shared_ptr<const ShadowSimulationObject>>>;

class BotObject : public SimulationObject
{
private:
    RangeValue<float> health;
    RangeValue<float> food;
    int see_distance;
    float speed;
    float damage;

    bool debug_drawing = false || DEFAULT_DEBUG_DRAWING;

    std::shared_ptr<ShadowBotObject> shadow;

    std::shared_ptr<ProtocolsHolder> protocolsHolder;

    std::shared_ptr<BotBrain> brain;

public:
    BotObject(std::shared_ptr<Simulation> simulation,
              Vec2<int> position,
              float health_,
              float food_,
              int see_distance_,
              float speed_,
              float damage_,
              float maxHealth = -1.0f,
              float maxFood = -1.0f);

    SimulationObjectType type() const override
    {
        return SimulationObjectType::BotObject;
    }

    /// @brief Getter for the shadow object (const version).
    /// @return A const shared pointer to the shadow object.
    std::shared_ptr<const ShadowBotObject> getShadow() const
    {
        return shadow;
    }

    void setID(unsigned long newID) override {
        id.set(newID);
        shadow->_id = newID;
        protocolsHolder->updateProtocol.body = shadow;
    }

    /// @brief Set brain for the bot and connect their protocols holders
    void setBrainObject(std::shared_ptr<BotBrain> brain_);

    /// @brief Return see distance of bot including chunk multiplier
    int getSeeDistance() const
    {
        if (auto validChunk = chunk.lock())
        {
            if (auto validSimulation = simulation.lock())
            {
                return std::min(static_cast<int>(see_distance * validChunk->getSeeDistanceMultiplier()),
                                validSimulation->maxSeeDistance);
            }
            throw std::runtime_error("Invalid simulation pointer of BotObject!");
        }
        throw std::runtime_error("Invalid chunk pointer of BotObject!");
    }

    /// @brief Get and return all chunks within a given radius of the position.
    /// @param position The position to check around.
    /// @param radius The radius within which to search for chunks.
    /// @return A vector of shared pointers to the chunks within the specified radius.
    std::vector<std::shared_ptr<Chunk>> getChunksInRadius(const Vec2<int> &position, int radius)
    {
        std::vector<std::shared_ptr<Chunk>> chunks;

        if (auto validSimulation = simulation.lock())
        {
            // Find top left chunk in radius
            std::shared_ptr<Chunk> topLeftChunk = validSimulation->chunkManager->whatChunkHere(position - radius);
            // Find bottom right chunk in radius
            std::shared_ptr<Chunk> bottomRightChunk = validSimulation->chunkManager->whatChunkHere(position + radius);

            // Get index of edge chunks(if radius out of map, return edge chunks)
            int startX = 0;
            int startY = 0;
            if (topLeftChunk)
            {
                startX = topLeftChunk->xIndex;
                startY = topLeftChunk->yIndex;
            }
            int endX = validSimulation->chunkManager->numberOfChunksX - 1;
            int endY = validSimulation->chunkManager->numberOfChunksY - 1;
            if (bottomRightChunk)
            {
                endX = bottomRightChunk->xIndex;
                endY = bottomRightChunk->yIndex;
            }
            // chunks.resize((endY - startY + 1) * (endX - startX + 1));
            for (int y = startY; y <= endY; y++)
            {
                for (int x = startX; x <= endX; x++)
                {
                    chunks.push_back(validSimulation->chunkManager->getChunk(x, y));
                }
            }
        }
        else
        {
            throw std::runtime_error("Invalid simulation pointer of BotObject!");
        }

        return chunks;
    }

    /// @brief Get all objects shadows within the bot's vision range.
    void packProtocol();

    /// @brief Check if the given object is within the bot's vision range.
    /// @param object The object to check.
    /// @param sqrSeeDistance getSeeDistance() * getSeeDistance() value
    /// @return Returns `true` if the object is within the bot's vision range, otherwise `false`.
    bool isInVision(const std::shared_ptr<SimulationObject> &object, int sqrSeeDistance) const
    {
        // Maybe we will need to change this function to take objects radius in account
        return pos.sqrDistanceTo(object->pos) <= sqrSeeDistance;
    }

    void update() override;

    void draw(ImDrawList *draw_list, ImVec2 drawing_delta_pos) override
    {
        draw_list->AddCircleFilled(ImVec2(drawing_delta_pos.x + pos.x, drawing_delta_pos.y + pos.y), getRadius(), color, 24);
        if (debug_drawing)
        {
            float radius_ = getRadius();
            float center_x = drawing_delta_pos.x + pos.x;
            float center_y = drawing_delta_pos.y + pos.y;
            constexpr int bar_height = 10;
            constexpr float bar_size_reduction = 0.3f;
            // Draw see distance circle
            draw_list->AddCircle(ImVec2(center_x, center_y), float(getSeeDistance()),
                                 ImColor(colorInt(255, 255, 255, 100)), 24, 1.0f);
            // Draw food bar
            draw_list->AddRectFilled(ImVec2(center_x - radius_, center_y - radius_),
                                     ImVec2(center_x - radius_ + food.getMax() * bar_size_reduction, center_y - radius_ - bar_height),
                                     ImColor(colorInt(0, 100, 0, 50)));
            draw_list->AddRectFilled(ImVec2(center_x - radius_, center_y - radius_),
                                     ImVec2(center_x - radius_ + food.getMax() * food.normalize() * bar_size_reduction, center_y - radius_ - bar_height),
                                     ImColor(colorInt(0, 200, 0, 50)));
            // Draw health bar
            draw_list->AddRectFilled(ImVec2(center_x - radius_, center_y - radius_ - bar_height),
                                     ImVec2(center_x - radius_ + health.getMax() * bar_size_reduction, center_y - radius_ - bar_height * 2),
                                     ImColor(colorInt(100, 0, 0, 50)));
            draw_list->AddRectFilled(ImVec2(center_x - radius_, center_y - radius_ - bar_height),
                                     ImVec2(center_x - radius_ + health.getMax() * health.normalize() * bar_size_reduction, center_y - radius_ - bar_height * 2),
                                     ImColor(colorInt(200, 0, 0, 50)));
        }
    }

    void displayInfo();

    void onDestroy() override;

    // Actions

    void parseProtocolResponce();

    void actionMove(Vec2<float> direction, float speedMultyplier = 1.0f);

    void actionGoTo(Vec2<float> targetPos);

    /// @brief Preform attack on specific bot
    /// @param attackOwnKind Indicate if bot would attack its own kind
    /// @param targetID If set, than bot will attack bot with given id, if can. If set to ULONG_MAX, will attack nearest bot
    void actionAttack(bool attackOwnKind = false, unsigned long targetID = ULONG_MAX);

    /// @brief Raw attack logic without any checks
    void rawAttack(std::shared_ptr<BotObject> targetBot);

    /// @brief Preform eating on specific food object
    /// @param targetID If set, than bot will food object bot with given id, if can. If set to ULONG_MAX, will eat nearest food object
    void actionEat(unsigned long targetID = ULONG_MAX);

    /// @brief Raw eat logic without any checks
    void rawEat(std::shared_ptr<FoodObject> targetFood);

    /// @brief Spawns a new bot if there is enough food and a valid simulation context.
    void actionSpawnBot(std::shared_ptr<BotBrain> brain, int evolutionPoints=-1);

    /// @brief Forces the bot to write its name in the DeathNote.
    void actionSuicide();
};