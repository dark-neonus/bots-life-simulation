#pragma once

#include <memory>

#include "simulation.h"
#include "chunks.h"
#include "utilities/utilities.h"

class Simulation;
class ObjectID;
class Chunk;

#ifndef SIMULATION_OBJECT_TYPE_ENUM
#define SIMULATION_OBJECT_TYPE_ENUM
enum class SimulationObjectType {
    BaseObject,
    BotObject,
    FoodObject,
    TreeObject,
};
#endif

const char* const SimulationObjectTypeNames[4] = {
    "BaseObject",
    "BotObject",
    "FoodObject",
    "TreeObject"
};

const char* getTypeString(SimulationObjectType type);

class SimulationObject : public std::enable_shared_from_this<SimulationObject>
{
private:
protected:
    std::weak_ptr<Simulation> simulation;
    // It would be better if SimulationObject::chunk stay protected
    std::weak_ptr<Chunk> chunk;

    int radius;
    ImU32 color;

    // bool highlighted = false;
public:
    Vec2<float> pos;

    ObjectID id;
    
    /// @brief Constructs a SimulationObject with the given parameters.
    /// @param simulation_ A shared pointer to the parent simulation. Used to interact with the simulation context.
    /// @param position The initial position of the object as a Vec2<float>.
    /// @param radius_ The radius of the object, defining its size.
    /// @param color_ The color of the object represented as an ImVec4 (RGBA format).
    SimulationObject(std::shared_ptr<Simulation> simulation_, Vec2<float> position, int radius_, ImVec4 color_)
        : simulation(simulation_), radius(radius_), color(ImColor(color_)), pos(position)
    {
    }

    virtual SimulationObjectType type() const {
        return SimulationObjectType::BaseObject;
    }

    virtual int getRadius() {
        return radius;
    }

    std::shared_ptr<Chunk> getChunk() {
        return chunk.lock();
    }

    void setChunk(std::shared_ptr<Chunk> chunkToSet) {
        chunk = chunkToSet; 
    }

    // bool isHighlighted() { return highlighted; }
    // void setHighlightion(bool value) { highlighted = value; }

    /// @brief Draw object to ImGui window
    /// @param draw_list Object to draw on provided by ImGui
    /// @param window_pos Position of window to draw on. Must add it to objects position
    virtual void draw(ImDrawList *draw_list, ImVec2 drawing_delta_pos)
    {
        draw_list->AddCircle(ImVec2(drawing_delta_pos.x + pos.x, drawing_delta_pos.y + pos.y), getRadius(), color, 24);
    }
    

    /// @brief Function to update object. Being called each frame
    virtual void update()
    {
        // pos = pos + Vec2<int>(rand() % 3 - 1, rand() % 3 - 1);
    }

    /// @brief Tell simulation to delete object at the end of frame
    virtual void markForDeletion();

    /// @brief Function that will be called before simulation destroy object
    virtual void onDestroy() {}

    virtual void displayInfo() {
        ImGui::SeparatorText("Simulation Object");
        ImGui::Text("ID: %0*lo:", 6, id.get());
        ImGui::Text("Position:");
        ImGui::InputFloat("x", &pos.x, 1.0f, 1.0f, "%.1f");
        ImGui::InputFloat("y", &pos.y, 1.0f, 1.0f, "%.1f");
    }

    const ImVec4 selectionColor = colorInt(255, 255, 255, 45);
    virtual void drawHighlightion(ImDrawList *draw_list, ImVec2 window_pos);

    virtual ~SimulationObject() = default;
};