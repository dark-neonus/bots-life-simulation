#pragma once

#include <memory>
#include <vector>

#include "imgui.h"
#include "utilities/utilities.h"

class Simulation;

class SimulationObject  : public std::enable_shared_from_this<SimulationObject>
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

    virtual int getRadius() { return radius; }

    /// @brief Draw object to ImGui window
    /// @param draw_list Object to draw on provided by ImGui
    /// @param window_delta Position of window to draw on. Must add it to objects position
    virtual void draw(ImDrawList *draw_list, ImVec2 window_delta)
    {
        draw_list->AddCircle(ImVec2(window_delta.x + pos.x, window_delta.y + pos.y), getRadius(), color, 24);
    }
    

    /// @brief Function to update object. Being called each frame
    virtual void update()
    {
        // pos = pos + Vec2<int>(rand() % 3 - 1, rand() % 3 - 1);
    }

    virtual void displayInfo() {
        ImGui::SeparatorText("Simulation Object");
        ImGui::Text("Position:");
        ImGui::InputFloat("x", &pos.x, 1.0f, 1.0f, "%.1f");
        ImGui::InputFloat("y", &pos.y, 1.0f, 1.0f, "%.1f");
    }
};

class Simulation
{
private:
    std::vector<std::shared_ptr<SimulationObject>> objects;

    std::weak_ptr<SimulationObject> viewInfoObject;

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
        ImVec2 object_center;
        ImVec2 mouse_pos = ImGui::GetMousePos();
        float dist_sq = 0.0f;
        for (auto& obj : objects)
        {
            obj->draw(draw_list, window_delta);
            // Check for click
            if (ImGui::IsMouseClicked(0)) {
                ImVec2 circle_center(window_delta.x + obj->pos.x, window_delta.y + obj->pos.y);
                float dist_sq = (mouse_pos.x - circle_center.x) * (mouse_pos.x - circle_center.x) + 
                        (mouse_pos.y - circle_center.y) * (mouse_pos.y - circle_center.y);
                if (dist_sq <= obj->getRadius() * obj->getRadius())
                {
                    setViewInfoObject(obj);
                }
            }
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
        auto obj = std::make_shared<T>(std::forward<Args>(args)...);
        T* objPtr = obj.get();
        objects.push_back(std::move(obj));

        return objPtr; // if you need to use raw pointer after adding.
    }

    /// @brief Sets the object to be displayed as the info view object.
    /// @param obj Pointer to the SimulationObject that should be set as the info view object.
    void setViewInfoObject(std::shared_ptr<SimulationObject> obj) {
        viewInfoObject = obj;  // Store as weak pointer
    }

    /// @brief Retrieves the current info view object.
    std::shared_ptr<SimulationObject> getInfoViewObject() {
        if (auto obj = viewInfoObject.lock()) {
            return obj;  // Lock the weak pointer to get the shared pointer
        }
        return nullptr;
    }
};
