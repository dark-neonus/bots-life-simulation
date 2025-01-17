#pragma once

#include "simulation.h"
#include "objects/SimulationObject.h"
#include "objects/Food.h"
#include "protocols/shadows/ShadowTreeObject.h"

class TreeObject : public SimulationObject
{
private:
    float foodMaxCalories;
    float foodGrowthRate;
    float foodDecayRate;
    bool foodIsMature;

    float foodSpawnCooldown;
    float foodSpawnCooldownMax;
protected:
    int numberOfFruits;
private:
    std::shared_ptr<ShadowTreeObject> shadow;
public:
    TreeObject(
        std::shared_ptr<Simulation> simulation,
        Vec2<float> position,
        int numberOfFruits_,
        float foodMaxCalories_,
        float foodGrowthRate_,
        float foodDecayRate_,
        float foodSpawnCooldownMax_,
        bool foodIsMature_)
        : SimulationObject(simulation, position, getRadius(), colorInt(60, 30, 0)),
        foodMaxCalories(foodMaxCalories_),
        foodGrowthRate(foodGrowthRate_),
        foodDecayRate(foodDecayRate_),
        foodIsMature(foodIsMature_),
        foodSpawnCooldown(foodSpawnCooldownMax_),
        foodSpawnCooldownMax(foodSpawnCooldownMax_),
        numberOfFruits(numberOfFruits_),
        shadow(std::make_shared<ShadowTreeObject>(id.get(), pos, getRadius(), numberOfFruits))
    {
    }

    SimulationObjectType type() const override {
        return SimulationObjectType::TreeObject;
    }

    /// @brief Getter for the shadow object (const version).
    /// @return A const shared pointer to the shadow object.
    std::shared_ptr<const ShadowTreeObject> getShadow() const
    {
        return shadow;
    }

    void update() override
    {
        if (foodSpawnCooldown > 0.0f) {
            foodSpawnCooldown -= 1.0f;
        }
        else {
            spawnFood();
            foodSpawnCooldown = foodSpawnCooldownMax;
        }

    }

    /// @brief Spawns a set of food objects around the Tree in a regular polygon pattern.
    void spawnFood() {
        for (int i = 0; i < numberOfFruits; ++i) {

            float angle = 2.0f * M_PI * i / numberOfFruits;

            Vec2<float> foodPosition = Vec2<float>(
                pos.x + cos(angle) * (getRadius() + 15),
                pos.y + sin(angle) * (getRadius() + 15)
            );

            if (auto validSimulation = simulation.lock()) {
                validSimulation->addObject(
                    SimulationObjectType::FoodObject,
                    std::make_shared<FoodObject>(
                        validSimulation, foodPosition,
                        colorInt(0, 255, 0),
                        foodMaxCalories,
                        foodMaxCalories * 0.1f,
                        foodGrowthRate,
                        foodDecayRate,
                        foodIsMature
                    )
                );
            }
        }
    }

    void draw(ImDrawList *draw_list, ImVec2 drawing_delta_pos, float zoom) override
    {
        draw_list->AddNgonFilled(ImVec2(pos.x * zoom + drawing_delta_pos.x, pos.y * zoom + drawing_delta_pos.y), getRadius() * zoom, color, numberOfFruits);
    }

    int getRadius() override { return 10 + numberOfFruits * 2; }

    void displayInfo() override {
        // Call parent class displayInfo to show basic information
        SimulationObject::displayInfo();

        // Add custom behavior for this class
        ImGui::SeparatorText("Tree Object");
        // TODO: Here in future we need to specify min and max value for number of fruits
        ImGui::SliderInt("Number of Fruits", &numberOfFruits, 3, 12);
    }
};