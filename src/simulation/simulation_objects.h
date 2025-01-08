#pragma once

class FoodObject : public SimulationObject
{
private:
protected:
    float calories;

public:
    FoodObject(std::shared_ptr<Simulation> simulation, Vec2<float> position, float calories_, ImVec4 color)
        : SimulationObject(simulation, position, getRadius(), color),
          calories(calories_)
    {
    }

    SimulationObjectTypes type() const override {
        return SimulationObjectTypes::FoodObject;
    }

    void update() override
    {
        // calories--;
        if (calories == 0) {
            markForDeletion();
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
protected:
    int numberOfFruits;

public:
    TreeObject(std::shared_ptr<Simulation> simulation, Vec2<float> position, int numberOfFruits_)
        : SimulationObject(simulation, position, getRadius(), colorInt(60, 30, 0)),
          numberOfFruits(numberOfFruits_)
    {
    }

    SimulationObjectTypes type() const override {
        return SimulationObjectTypes::TreeObject;
    }

    void update() override
    {
        // Do nothing for the moment
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