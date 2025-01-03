#pragma once

#include <memory>
#include <vector>

#include "imgui.h"
#include "utilities/utilities.h"

class Simulation;

class SimulationObject
{
private:
protected:
    std::weak_ptr<Simulation> simulation;

    int radius;
    ImU32 color;

public:
    Vec2<float> pos;
    
    /// @brief Constructs a SimulationObject with the given parameters.
    /// @param simulation_ A shared pointer to the parent simulation. Used to interact with the simulation context.
    /// @param position The initial position of the object as a Vec2<float>.
    /// @param radius_ The radius of the object, defining its size.
    /// @param color_ The color of the object represented as an ImVec4 (RGBA format).
    SimulationObject(std::shared_ptr<Simulation> simulation_, Vec2<float> position, int radius_, ImVec4 color_)
        : simulation(simulation_), radius(radius_), color(ImColor(color_)), pos(position)
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
    std::vector<std::unique_ptr<SimulationObject>> objects;

public:
    const int unit;

    Simulation(int unit_ = 10)
        : unit(unit_)
    {
    }

    /// @brief Update all objects in simulation
    void update()
    {
        for (auto& obj : objects)
        {
            obj->update();
        }
    }

    /// @brief Render all objects in simulation
    /// @param draw_list Object to draw on provided by ImGui
    /// @param window_delta Position of window to draw on. Must add it to objects position 
    void render(ImDrawList *draw_list, ImVec2 window_delta)
    {
        for (auto& obj : objects)
        {
            obj->draw(draw_list, window_delta);
        }
    }

    /// @brief Adds a new object to the simulation.
    /// @tparam T The type of object to add. Must derive from SimulationObject.
    /// @tparam Args Variadic template for the arguments required to construct the object.
    /// @param args Arguments to forward to the constructor of the object.
    /// @return A raw pointer to the newly created object.
    template <typename T, typename... Args>
    T* addObject(Args&&... args)
    {
        auto obj = std::make_unique<T>(std::forward<Args>(args)...);
        T* objPtr = obj.get();
        objects.push_back(std::move(obj));

        return objPtr; // if you need to use raw pointer after adding.
    }
};
