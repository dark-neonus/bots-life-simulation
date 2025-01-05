#pragma once

#include <memory>
#include <vector>
#include <stdexcept>

#include "imgui.h"
#include "utilities/utilities.h"
#include "chunks.h"

class Simulation;

class SimulationObject : public std::enable_shared_from_this<SimulationObject>
{
private:
protected:
    std::weak_ptr<Simulation> simulation;
    std::weak_ptr<Chunk> chunk;

    int radius;
    ImU32 color;

    bool highlighted = false;
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

    virtual int getRadius() {
        return radius;
    }

    std::shared_ptr<Chunk> getChunk() {
        return chunk.lock();
    }

    void setChunk(std::shared_ptr<Chunk> chunkToSet) {
        chunk = chunkToSet; 
    }

    bool isHighlighted() { return highlighted; }
    void setHighlightion(bool value) { highlighted = value; }

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

    const ImVec4 selectionColor = colorInt(255, 255, 255, 45);
    virtual void drawHighlightion(ImDrawList *draw_list, ImVec2 window_delta) {
        static constexpr int selectionRubHeigt = 3;
        static constexpr int selectionRubWidth = 2;
        Vec2<float> realPos = Vec2<float>(window_delta) + pos;
        int selectionRadius = getRadius() + 8;
        draw_list->AddRect(toImVec2(realPos - selectionRadius), toImVec2(realPos + selectionRadius), ImColor(selectionColor), 0, 0, 3.0f);
        // lines |
        // draw_list->AddRectFilled(toImVec2(realPos + Vec2<float>(-selectionRubWidth, selectionRadius - selectionRubHeigt)),
        //                             toImVec2(realPos + Vec2<float>(selectionRubWidth, selectionRadius + selectionRubHeigt)), ImColor(selectionColor));
        // draw_list->AddRectFilled(toImVec2(realPos + Vec2<float>(-selectionRubWidth, -selectionRadius - selectionRubHeigt)),
        //                             toImVec2(realPos + Vec2<float>(selectionRubWidth, -selectionRadius + selectionRubHeigt)), ImColor(selectionColor));
        // draw_list->AddRectFilled(toImVec2(realPos + Vec2<float>(selectionRadius - selectionRubHeigt, -selectionRubWidth)),
        //                             toImVec2(realPos + Vec2<float>(selectionRadius + selectionRubHeigt, selectionRubWidth)), ImColor(selectionColor));
        // draw_list->AddRectFilled(toImVec2(realPos + Vec2<float>(-selectionRadius - selectionRubHeigt, -selectionRubWidth)),
        //                             toImVec2(realPos + Vec2<float>(-selectionRadius + selectionRubHeigt, selectionRubWidth)), ImColor(selectionColor));
    }
};

class Simulation : public std::enable_shared_from_this<Simulation>
{
private:
    std::vector<std::shared_ptr<SimulationObject>> objects;

    std::weak_ptr<SimulationObject> viewInfoObject;

public:
    // This property must be first
    const int unit;

    ChunkManager chunkManager;

    
    const int maxSeeDistance;


    Simulation(int numberOfChunksX_, int numberOfChunksY_, int unit_ = 10)
        : unit(unit_),
        chunkManager(numberOfChunksX_, numberOfChunksY_, float(unit * 10)),
        maxSeeDistance(chunkManager.chunkSize * 1.4)
    {
        // chunkManager.simulation = shared_from_this();
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
    void render(ImDrawList *draw_list, ImVec2 window_delta, bool drawDebugLayer=true)
    {
        chunkManager.drawChunksMesh(draw_list, window_delta);

        ImVec2 mouse_pos = ImGui::GetMousePos();
        // Holder variable for distance between click position and object
        float dist_sq = 0.0f;
        ImVec2 object_center;
        // In future, when we will have camera will be able to move it,
        // this should be optimized to call Chunk::draw() if chunk is in visible area
        // And then Chunk::draw() will call drawing of all all chunks objects
        for (auto& obj : objects)
        {
            obj->draw(draw_list, window_delta);
            obj->setHighlightion(false);
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
        if (auto viewInfoObjectValid = viewInfoObject.lock()) {
            viewInfoObjectValid->setHighlightion(true);
        }
        if (drawDebugLayer) {
            for (auto& obj : objects)
            {
                if (obj->isHighlighted()) {
                    obj->drawHighlightion(draw_list, window_delta);
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

        // Check if position is valid (inside map)
        if (obj->pos.x < 0 || obj->pos.y < 0 ||
            obj->pos.x > chunkManager.mapWidth || obj->pos.y > chunkManager.mapHeight) {
            throw std::invalid_argument("Position of object is out of simualtion map. Pos: " + obj->pos.text());
        }
        // !!!! This must be working code, just uncomment it !!!!
        
        // Assign chunk to object and object to chunk
        std::shared_ptr<Chunk> objectsChunk = chunkManager.whatChunkHere(obj->pos);
        if (objectsChunk) {
            objectsChunk->addObject<T>(obj);
            // obj->setChunk(objectsChunk)
        }
        else {
            throw std::invalid_argument("No chunk found for the given position. Pos: " + obj->pos.text());
        }

        objects.push_back(obj);
        return obj.get();
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

    void drawHighlight(std::weak_ptr<SimulationObject> obj, ImDrawList *draw_list, ImVec2 window_delta) {

    }
};
