#pragma once

#include "simulation.h"

class FoodObject : public SimulationObject
{
private:
protected:
    int calories;

public:
    FoodObject(Vec2<float> position, int calories_, ImVec4 color)
        : SimulationObject(position, converCaloriesToRadius(calories_), color),
          calories(calories_)
    {
    }

    void update() override
    {
        // do nothing
    }

    void draw(ImDrawList *draw_list, ImVec2 window_delta) override
    {
        draw_list->AddRectFilled(ImVec2(window_delta.x + pos.x - radius, window_delta.y + pos.y - radius),
                                 ImVec2(window_delta.x + pos.x + radius, window_delta.y + pos.y + radius),
                                 color);
    }
};

class TreeObject : public SimulationObject
{
private:
protected:
    int numberOfFruits;

public:
    TreeObject(Vec2<float> position, int numberOfFruits_)
        : SimulationObject(position, 10 + numberOfFruits_ * 2, colorInt(80, 40, 0)),
          numberOfFruits(numberOfFruits_)
    {
    }

    void update() override
    {
        // Do nothing for the moment
    }

    void draw(ImDrawList *draw_list, ImVec2 window_delta) override
    {
        draw_list->AddNgonFilled(ImVec2(pos.x + window_delta.x, pos.y + window_delta.y), radius, color, numberOfFruits);
    }
};
