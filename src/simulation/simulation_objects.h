#pragma once

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

public:
    TreeObject(std::shared_ptr<Simulation> simulation, Vec2<float> position, int numberOfFruits_,
        float foodMaxCalories_, float foodGrowthRate_, float foodDecayRate_, float foodSpawnCooldownMax_, bool foodIsMature_)
        : SimulationObject(simulation, position, getRadius(), colorInt(60, 30, 0)),
        foodMaxCalories(foodMaxCalories_),
        foodGrowthRate(foodGrowthRate_),
        foodDecayRate(foodDecayRate_),
        foodIsMature(foodIsMature_),
        foodSpawnCooldown(foodSpawnCooldownMax_),
        foodSpawnCooldownMax(foodSpawnCooldownMax_),
        numberOfFruits(numberOfFruits_)
    {
    }

    SimulationObjectTypes type() const override {
        return SimulationObjectTypes::TreeObject;
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

    void spawnFood() {
        for (int i = 0; i < numberOfFruits; ++i) {

            float angle = 2.0f * M_PI * i / numberOfFruits;

            Vec2<float> foodPosition = Vec2<float>(
                pos.x + cos(angle) * (getRadius() + 15),
                pos.y + sin(angle) * (getRadius() + 15)
            );

            if (auto validSimulation = simulation.lock()) {
                validSimulation->addObject<FoodObject>(
                    validSimulation, foodPosition,
                    colorInt(0, 255, 0),
                    foodMaxCalories,
                    foodMaxCalories * 0.1f,
                    foodGrowthRate,
                    foodDecayRate,
                    foodIsMature
                );
            }
        }
    }

    void draw(ImDrawList *draw_list, ImVec2 drawing_delta_pos) override
    {
        draw_list->AddNgonFilled(ImVec2(pos.x + drawing_delta_pos.x, pos.y + drawing_delta_pos.y), getRadius(), color, numberOfFruits);
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