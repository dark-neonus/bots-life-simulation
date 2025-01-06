#pragma once

#include "chunks.h"
#include "simulation.h"
#include "simulation_objects.h"
#include "utilities/utilities.h"

#define DEFAULT_DEBUG_DRAWING true

std::shared_ptr<Chunk> whatChunkHere(Vec2<float> position);

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
        SimulationObject(simulation, position, convertCaloriesToRadius(food_), colorInt(0, 125, 255))
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
            throw std::runtime_error("Invalid simulation of BotObject!");
        }
        throw std::runtime_error("Invalid chunk of BotObject!");
    }

    /// @brief Get and return all chunks within a given radius of the position.
    /// @param position The position to check around.
    /// @param radius The radius within which to search for chunks.
    /// @return A vector of shared pointers to the chunks within the specified radius.
    std::vector<std::shared_ptr<Chunk>> getChunksInRadius(const Vec2<int>& position, int radius) {
        std::vector<std::shared_ptr<Chunk>> chunks;
        
        Vec2<int> min = position - Vec2<int>(radius, radius);
        Vec2<int> max = position + Vec2<int>(radius, radius);

        for (int x = min.x; x <= max.x; ++x) {
            for (int y = min.y; y <= max.y; ++y) {
                auto chunk = whatChunkHere(Vec2<int>(x, y));
                if (chunk) {
                    chunks.push_back(chunk);
                }
            }
        }

        return chunks;
    }

    /// @brief Get all objects within the bot's vision range.
    /// @return A set of weak pointers to the objects in the bot's vision.
    objectSet getObjectsInVision() {
        objectSet objectsInVision;

        const int radius = getSeeDistance();
        auto chunksInVision = getChunksInRadius(pos, radius);

        for (const auto& chunk : chunksInVision) {
            for (const auto& weakObject : chunk->getObjects()) {
                if (auto object = weakObject.lock()) {
                    if (isInVision(object)) {
                        objectsInVision.insert(object);
                    }
                }
            }
        }

        return objectsInVision;
    }

    /// @brief Check if the given object is within the bot's vision range.
    /// @param object The object to check.
    /// @return Returns `true` if the object is within the bot's vision range, otherwise `false`.
    bool isInVision(const std::shared_ptr<SimulationObject>& object) const {
        return pos.distanceTo(object->pos) <= getSeeDistance();
    }

    void update() override
    {
        // Change values just to display debug values drawing
        if (debug_drawing)
        {
            food.decrease(0.1);
            if (food.get() == 0)
            {
                health.decrease(0.5);
            }
        }
    }

    void draw(ImDrawList* draw_list, ImVec2 window_pos) override
    {
        draw_list->AddCircleFilled(ImVec2(window_pos.x + pos.x, window_pos.y + pos.y), getRadius(), color, 24);
        if (debug_drawing)
        {
            float radius_ = getRadius();
            float center_x = window_pos.x + pos.x;
            float center_y = window_pos.y + pos.y;
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
};