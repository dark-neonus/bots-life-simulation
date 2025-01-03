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

    void update() override
    {
        // do nothing
    }

    void draw(ImDrawList *draw_list, ImVec2 window_delta) override
    {
        draw_list->AddRectFilled(ImVec2(window_delta.x + pos.x - getRadius(), window_delta.y + pos.y - getRadius()),
                                 ImVec2(window_delta.x + pos.x + getRadius(), window_delta.y + pos.y + getRadius()),
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
        : SimulationObject(simulation, position, getRadius(), colorInt(80, 40, 0)),
          numberOfFruits(numberOfFruits_)
    {
    }

    void update() override
    {
        // Do nothing for the moment
    }

    void draw(ImDrawList *draw_list, ImVec2 window_delta) override
    {
        draw_list->AddNgonFilled(ImVec2(pos.x + window_delta.x, pos.y + window_delta.y), getRadius(), color, numberOfFruits);
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