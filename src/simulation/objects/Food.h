#pragma once

#include "simulation.h"
#include "objects/SimulationObject.h"

class FoodObject : public SimulationObject
{
private:
    const float maxCalories;
    const float growthRate;
    const float decayRate;
    bool isMature;
protected:
    float calories;

public:
    FoodObject(std::shared_ptr<Simulation> simulation, Vec2<float> position, ImVec4 color,
        float maxCalories_, float calories_, float growthRate_, float decayRate_, bool isMature_)
        : SimulationObject(simulation, position, getRadius(), color),
        maxCalories(maxCalories_),
        growthRate(growthRate_),
        decayRate(decayRate_),
        isMature(isMature_),
        calories(calories_)
    {
    }
     
    SimulationObjectType type() const override {
        return SimulationObjectType::FoodObject;
    }

    void update() override {
        if (!isMature) {
            calories += growthRate;
            if (calories >= maxCalories) {
                calories = maxCalories;
                isMature = true;
            }
        }
        else {
            calories -= decayRate;
            if (calories <= 0) {
                markForDeletion();
            }
        }
    }

    void draw(ImDrawList *draw_list, ImVec2 drawing_delta_pos) override
    {
        draw_list->AddRectFilled(ImVec2(drawing_delta_pos.x + pos.x - getRadius(), drawing_delta_pos.y + pos.y - getRadius()),
                                 ImVec2(drawing_delta_pos.x + pos.x + getRadius(), drawing_delta_pos.y + pos.y + getRadius()),
                                 color);
    }

    int getRadius() override { return convertCaloriesToRadius(calories); }

    void displayInfo() override {
        // Call parent class displayInfo to show basic information
        SimulationObject::displayInfo();

        // Add custom behavior for this class
        ImGui::SeparatorText("Food Object");
        // TODO: Here in future we need to specify min and max value for calories
        ImGui::SliderFloat("Calories", &calories, 0.0f, 500.0f, "%.1f");
    }
};
