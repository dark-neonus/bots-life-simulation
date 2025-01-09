#pragma once

#include "simulation.h"
#include "objects/SimulationObject.h"
#include "utilities/utilities.h"

class FoodObject : public SimulationObject
{
private:
    RangeValue<float> calories;

    float growthRate;
    float decayRate;

    bool isMature;

public:
    FoodObject(std::shared_ptr<Simulation> simulation, Vec2<float> position, ImVec4 color,
        float maxCalories_, float calories_, float growthRate_, float decayRate_, bool isMature_)
        : SimulationObject(simulation, position, getRadius(), color),
        calories(calories_, 0.0f, maxCalories_),
        growthRate(growthRate_),
        decayRate(decayRate_),
        isMature(isMature_)
    {
    }
     
    SimulationObjectType type() const override {
        return SimulationObjectType::FoodObject;
    }


    float decreaseCalories(float amount) {
        if (amount < 0) {
            throw std::invalid_argument("Calories reduction amount cant be negative!");
        }
        float decreasedAmount = std::min(amount, calories.get());
        calories.decrease(amount);
        if (calories.get() == 0.0f) {
            markForDeletion();
        }
        return decreasedAmount;
    }

    void update() override {
        if (!isMature) {
            calories.increase(growthRate);
            if (calories.get() >= calories.getMax()) {
                isMature = true;
            }
        }
        else {
            decreaseCalories(decayRate);
        }
    }

    void draw(ImDrawList *draw_list, ImVec2 drawing_delta_pos) override
    {
        draw_list->AddRectFilled(ImVec2(drawing_delta_pos.x + pos.x - getRadius(), drawing_delta_pos.y + pos.y - getRadius()),
                                 ImVec2(drawing_delta_pos.x + pos.x + getRadius(), drawing_delta_pos.y + pos.y + getRadius()),
                                 color);
    }

    int getRadius() override { return convertCaloriesToRadius(calories.get()); }

    void displayInfo() override {
        // Call parent class displayInfo to show basic information
        SimulationObject::displayInfo();

        // Add custom behavior for this class
        ImGui::SeparatorText("Food Object");
        // TODO: Here in future we need to specify min and max value for calories
        ImGui::SliderFloat("Calories", calories.valuePointer(), calories.getMin(), calories.getMax(), "%.1f");
        ImGui::SliderFloat("GrowthRate", &growthRate, 0.0f, 50.0f, "%.1f");
        ImGui::SliderFloat("DecayRate", &decayRate, 0.0f, 50.0f, "%.1f");
        ImGui::Checkbox("Mature", &isMature);
    }
};
