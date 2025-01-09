#pragma once

#include <memory>

#include "chunks.h"
#include "simulation.h"
#include "objects/SimulationObject.h"
#include "utilities/utilities.h"

class FoodObject;

#define DEFAULT_DEBUG_DRAWING true

using objectSet = std::unordered_set<std::weak_ptr<SimulationObject>,
    std::hash<std::weak_ptr<SimulationObject>>,
    std::equal_to<std::weak_ptr<SimulationObject>>>;

class BotObject : public SimulationObject
{
private:
    RangeValue<float> health;
    RangeValue<float> food;
    int see_distance;
    float speed;
    float damage;

    bool debug_drawing = false || DEFAULT_DEBUG_DRAWING;

protected:
public:
    BotObject(std::shared_ptr<Simulation> simulation, Vec2<int> position, float health_, float food_, int see_distance_, float speed_, float damage_)
        : health(health_, 0, health_), food(food_, 0, food_),
        see_distance(see_distance_), speed(speed_), damage(damage_),
        SimulationObject(simulation, position, convertCaloriesToRadius(food_), colorInt(0, 75, 150))
    {
    }

    SimulationObjectType type() const override {
        return SimulationObjectType::BotObject;
    }

    /// @brief Return see distance of bot including chunk multiplier
    int getSeeDistance() const {
        if (auto validChunk = chunk.lock()) {
            if (auto validSimulation = simulation.lock()) {
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
    std::vector<std::shared_ptr<Chunk>> getChunksInRadius(const Vec2<int>& position, int radius) {
        std::vector<std::shared_ptr<Chunk>> chunks;

        if (auto validSimulation = simulation.lock()) {
            // Find top left chunk in radius
            std::shared_ptr<Chunk> topLeftChunk = validSimulation->chunkManager->whatChunkHere(position - radius);
            // Find bottom right chunk in radius
            std::shared_ptr<Chunk> bottomRightChunk = validSimulation->chunkManager->whatChunkHere(position + radius);

            // Get index of edge chunks(if radius out of map, return edge chunks)
            int startX = 0;
            int startY = 0;
            if (topLeftChunk) {
                startX = topLeftChunk->xIndex;
                startY = topLeftChunk->yIndex;
            }
            int endX = validSimulation->chunkManager->numberOfChunksX - 1;
            int endY = validSimulation->chunkManager->numberOfChunksY - 1;
            if (bottomRightChunk) {
                endX = bottomRightChunk->xIndex;
                endY = bottomRightChunk->yIndex;
            }
            // chunks.resize((endY - startY + 1) * (endX - startX + 1));
            for (int y = startY; y <= endY; y++) {
                for (int x = startX; x <= endX; x++) {
                    chunks.push_back(validSimulation->chunkManager->getChunk(x, y));
                }
            }

        }
        else {
            throw std::runtime_error("Invalid simulation pointer of BotObject!");
        }
        

        return chunks;
    }

    /// @brief Get all objects within the bot's vision range.
    /// @return A set of weak pointers to the objects in the bot's vision.
    objectSet getObjectsInVision() {
        // It may be faster to use std::vector<std::shared_ptr<SimulationObject>>
        objectSet objectsInVision;

        const int radius = getSeeDistance();
        const int sqrSeeDistance = radius * radius;
        auto chunksInVision = getChunksInRadius(pos, radius);

        for (const auto& chunk : chunksInVision) {
            for (const auto& chunkObject : chunk->getObjects()) {
                if (auto validChunkObject = chunkObject.lock()) {
                    if (isInVision(validChunkObject, sqrSeeDistance) && validChunkObject.get() != this) {
                        objectsInVision.insert(validChunkObject);
                    }
                }
            }
        }

        // Just leaving it here. To run need make Simulation::objects public
        // objectSet objectsInVision;

        // const int radius = getSeeDistance();
        // const int sqrSeeDistance = radius * radius;
        // // auto chunksInVision = getChunksInRadius(pos, radius);

        // if (auto validSimulation = simulation.lock()) {
        //     for (auto chunkObject : validSimulation->objects) {
        //         if (isInVision(chunkObject, sqrSeeDistance) && chunkObject.get() != this) {
        //             objectsInVision.insert(chunkObject);
        //         }
        //     }
        // }

        return objectsInVision;
    }

    /// @brief Check if the given object is within the bot's vision range.
    /// @param object The object to check.
    /// @param sqrSeeDistance getSeeDistance() * getSeeDistance() value
    /// @return Returns `true` if the object is within the bot's vision range, otherwise `false`.
    bool isInVision(const std::shared_ptr<SimulationObject>& object, int sqrSeeDistance) const {
        // Maybe we will need to change this function to take objects radius in account
        return pos.sqrDistanceTo(object->pos) <= sqrSeeDistance;
    }

    void update() override
    {
        // Change values just to display debug values drawing
        if (debug_drawing)
        {
            // food.decrease(0.1);
            if (food.get() == 0)
            {
                health.decrease(0.5);
            } else if (food.get() > food.getMax() * 0.3f) {
                //actionSpawnBot();
                //actionSuicide();
            }
        }
        auto objectsInVision = getObjectsInVision();
        if (health.get() == 0) {
            markForDeletion();
        }
        else if (health.get() < health.getMax() && food.get() >= food.getMax() / 2) {
            // Healing logic
            health.increase(0.1);
            food.decrease(0.2);
        }
    }

    void draw(ImDrawList* draw_list, ImVec2 drawing_delta_pos) override
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

    void displayInfo() override {
        // Call parent class displayInfo to show basic information
        SimulationObject::displayInfo();

        // Add custom behavior for this class
        ImGui::SeparatorText("Bot Object");
        // ImGui::InputFloat("Custom Value", &customValue, 1.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Health", health.valuePointer(), health.getMin(), health.getMax(), "%.1f");
        ImGui::SliderFloat("Food", food.valuePointer(), food.getMin(), food.getMax(), "%.1f calories");
        // TODO: Here in future we need to specify min and max value for see distance
        if (auto simulationValid = simulation.lock()) {
            ImGui::SliderInt("See distance", &see_distance, 1, simulationValid->maxSeeDistance);
        }
        // TODO: Here in future we need to specify min and max value for speed
        ImGui::SliderFloat("Speed", &speed, 0.1f, 10.0f, "%.2f");
        // TODO: Here in future we need to specify min and max value for damage
        ImGui::SliderFloat("Damage", &damage, 0.0f, 20.0f, "%.2f");
        ImGui::Checkbox("Debug drawing", &debug_drawing);
    }

    void onDestroy() override;

    // Actions

    void actionMove(Vec2<float> direction, float speedMultyplier = 1.0f);

    /// @brief Preform attack on specific bot
    /// @param attackOwnKind Indicate if bot would attack its own kind
    /// @param targetID If set, than bot will attack bot with given id, if can. If set to ULONG_MAX, will attack nearest bot
    void actionAttack(bool attackOwnKind=false, unsigned long targetID=ULONG_MAX);

    /// @brief Raw attack logic without any checks
    void rawAttack(std::shared_ptr<BotObject> targetBot);

    /// @brief Preform eating on specific food object
    /// @param targetID If set, than bot will food object bot with given id, if can. If set to ULONG_MAX, will eat nearest food object
    void actionEat(unsigned long targetID=ULONG_MAX);

    /// @brief Raw eat logic without any checks
    void rawEat(std::shared_ptr<FoodObject> targetFood);

    /// @brief Spawns a new bot if there is enough food and a valid simulation context.
    void actionSpawnBot();

    /// @brief Forces the bot to write its name in the DeathNote.
    void actionSuicide();

};