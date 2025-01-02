#pragma once

#include "simulation.h"
#include "simulation_objects.h"
#include "utilities/utilities.h"

#define DEBUG_DRAWING true

class BotObject : public SimulationObject
{
private:
    RangeValue<float> health;
    RangeValue<float> food;
    int see_distance;
    float speed;
    float damage;

    bool debug_drawing = false || DEBUG_DRAWING;

protected:
public:
    BotObject(Vec2<int> position, float health_, float food_, int see_distance_, float speed_, float damage_)
        : health(health_, 0, health_), food(food_, 0, food_),
          see_distance(see_distance_), speed(speed_), damage(damage_),
          SimulationObject(position, converCaloriesToRadius(food_), colorInt(0, 125, 255))
    {
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

    void draw(ImDrawList *draw_list, ImVec2 window_delta) override
    {
        draw_list->AddCircleFilled(ImVec2(window_delta.x + pos.x, window_delta.y + pos.y), radius, color, 24);
        if (debug_drawing)
        {
            float center_x = window_delta.x + pos.x;
            float center_y = window_delta.y + pos.y;
            constexpr int bar_height = 10;
            constexpr float bar_size_reduction = 0.3f;
            // Draw see distance circle
            draw_list->AddCircle(ImVec2(center_x, center_y), float(see_distance),
                                 ImColor(colorInt(255, 255, 255, 100)), 24, 1.0f);
            // Draw food bar
            draw_list->AddRectFilled(ImVec2(center_x - radius, center_y - radius),
                                     ImVec2(center_x - radius + food.getMax() * bar_size_reduction, center_y - radius - bar_height),
                                     ImColor(colorInt(0, 100, 0, 50)));
            draw_list->AddRectFilled(ImVec2(center_x - radius, center_y - radius),
                                     ImVec2(center_x - radius + food.getMax() * food.normalize() * bar_size_reduction, center_y - radius - bar_height),
                                     ImColor(colorInt(0, 200, 0, 50)));
            // Draw health bar
            draw_list->AddRectFilled(ImVec2(center_x - radius, center_y - radius - bar_height),
                                     ImVec2(center_x - radius + health.getMax() * bar_size_reduction, center_y - radius - bar_height * 2),
                                     ImColor(colorInt(100, 0, 0, 50)));
            draw_list->AddRectFilled(ImVec2(center_x - radius, center_y - radius - bar_height),
                                     ImVec2(center_x - radius + health.getMax() * health.normalize() * bar_size_reduction, center_y - radius - bar_height * 2),
                                     ImColor(colorInt(200, 0, 0, 50)));
        }
    }
};
