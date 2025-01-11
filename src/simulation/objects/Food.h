#pragma once

#include "simulation.h"
#include "objects/SimulationObject.h"
#include "utilities/utilities.h"
#include "protocols/shadows/ShadowFoodObject.h"

class FoodObject : public SimulationObject
{
private:
    RangeValue<float> calories;

    float growthRate;
    float decayRate;

    Counter growingTime;
    Counter matureTime;

    std::shared_ptr<ShadowFoodObject> shadow;

public:
    FoodObject(
        std::shared_ptr<Simulation> simulation,
        Vec2<float> position,
        ImVec4 color,
        float maxCalories_,
        float calories_,
        float growthRate_,
        float decayRate_,
        bool isMature_)
        : SimulationObject(simulation, position, getRadius(), color),
          calories(calories_, 0.0f, maxCalories_),
          growthRate(growthRate_),
          decayRate(decayRate_),
          growingTime(isMature_ ? int(calories.getMax() / growthRate) : 0, int(calories.getMax() / growthRate)),
          matureTime(isMature_ ? 50 : 0, 50),
          shadow(std::make_shared<ShadowFoodObject>(id.get(), pos, getRadius(),
                                                    calories.get(), !growingTime.isMax(),
                                                    growingTime.isMax() && matureTime.isMax()))
    {
    }

    SimulationObjectType type() const override
    {
        return SimulationObjectType::FoodObject;
    }

    /// @brief Getter for the shadow object (const version).
    /// @return A const shared pointer to the shadow object.
    std::shared_ptr<const ShadowFoodObject> getShadow() const
    {
        return shadow;
    }

    float decreaseCalories(float amount)
    {
        if (amount < 0)
        {
            throw std::invalid_argument("Calories reduction amount cant be negative!");
        }
        float decreasedAmount = std::min(amount, calories.get());
        calories.decrease(amount);
        if (calories.get() == 0.0f)
        {
            markForDeletion();
        }
        shadow->_calories = calories.get();
        return decreasedAmount;
    }

    void update() override
    {
        if (growingTime.isMax())
        {
            if (matureTime.isMax())
            {
                calories.decrease(decayRate);
            }
            else
            {
                matureTime.increment();
            }
        }
        else
        {
            growingTime.increment();
            calories.increase(growthRate);
        }
        if (calories.get() == 0.0f)
        {
            markForDeletion();
        }
        shadow->_calories = calories.get();
        shadow->_isGrowing = growingTime.isMax();
        shadow->_isDecaying = growingTime.isMax() && matureTime.isMax();
        shadow->_radius = getRadius();
    }

    void draw(ImDrawList *draw_list, ImVec2 drawing_delta_pos) override
    {
        draw_list->AddRectFilled(ImVec2(drawing_delta_pos.x + pos.x - getRadius(), drawing_delta_pos.y + pos.y - getRadius()),
                                 ImVec2(drawing_delta_pos.x + pos.x + getRadius(), drawing_delta_pos.y + pos.y + getRadius()),
                                 color);
    }

    int getRadius() override { return convertCaloriesToRadius(calories.get()); }

    void displayInfo() override
    {
        // Call parent class displayInfo to show basic information
        SimulationObject::displayInfo();

        // Add custom behavior for this class
        ImGui::SeparatorText("Food Object");
        // TODO: Here in future we need to specify min and max value for calories
        ImGui::SliderFloat("Calories", calories.valuePointer(), calories.getMin(), calories.getMax(), "%.1f");
        ImGui::SliderInt("GrowingTime", growingTime.valuePointer(), growingTime.getMin(), growingTime.getMax());
        ImGui::SliderFloat("GrowthRate", &growthRate, 0.0f, 50.0f, "%.1f");
        ImGui::SliderInt("MatureTime", matureTime.valuePointer(), matureTime.getMin(), matureTime.getMax());
        ImGui::SliderFloat("DecayRate", &decayRate, 0.0f, 50.0f, "%.1f");
    }
};
