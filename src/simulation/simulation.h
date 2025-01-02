#pragma once

#include <memory>
#include <vector>

#include "imgui.h"
#include "utilities/utilities.h"

class SimulationObject
{
private:
protected:
    int radius;
    ImU32 color;

    // std::shared_ptr<Simulation> simulation;

public:
    Vec2<float> pos;

    SimulationObject(Vec2<float> position, int radius_, ImVec4 color_)
        : pos(position), radius(radius_), color(ImColor(color_))
    {
    }

    /// @brief Draw object to ImGui window
    /// @param draw_list Object to draw on provided by ImGui
    /// @param window_delta Position of window to draw on. Must add it to objects position 
    virtual void draw(ImDrawList *draw_list, ImVec2 window_delta)
    {
        draw_list->AddCircle(ImVec2(window_delta.x + pos.x, window_delta.y + pos.y), radius, color, 24);
    }

    /// @brief Function to update object. Being called each frame
    virtual void update()
    {
        // pos = pos + Vec2<int>(rand() % 3 - 1, rand() % 3 - 1);
    }
};

class Simulation
{
private:
    std::vector<SimulationObject *> objects;

public:
    const int unit;

    Simulation(int unit_ = 10)
        : unit(unit_)
    {
    }

    /// @brief Update all objects in simulation
    void update()
    {
        for (auto obj : objects)
        {
            obj->update();
        }
    }

    /// @brief Render all objects in simulation
    /// @param draw_list Object to draw on provided by ImGui
    /// @param window_delta Position of window to draw on. Must add it to objects position 
    void render(ImDrawList *draw_list, ImVec2 window_delta)
    {
        for (auto obj : objects)
        {
            obj->draw(draw_list, window_delta);
        }
    }

    /// @brief Add object to simulation
    /// @param obj Pointer to object to add to simulation.
    void addObject(SimulationObject *obj)
    {
        objects.push_back(obj);
    }
};
