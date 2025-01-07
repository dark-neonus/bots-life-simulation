#pragma once

#include <vector>
#include <stdexcept>

#include "imgui.h"
#include "utilities/utilities.h"
#include "chunks.h"

using objectSet = std::unordered_set<std::weak_ptr<SimulationObject>,
    std::hash<std::weak_ptr<SimulationObject>>,
    std::equal_to<std::weak_ptr<SimulationObject>>>;

class Simulation;

enum class SimulationObjectType {
    BaseObject,
    BotObject,
    FoodObject,
    TreeObject,
};

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
    virtual void draw(ImDrawList *draw_list, ImVec2 window_pos)
    {
        draw_list->AddCircle(ImVec2(window_pos.x + pos.x, window_pos.y + pos.y), getRadius(), color, 24);
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
    virtual void drawHighlightion(ImDrawList *draw_list, ImVec2 window_pos);
};

class Simulation : public std::enable_shared_from_this<Simulation>
{
private:
    std::vector<std::shared_ptr<SimulationObject>> objects;

    // std::weak_ptr<SimulationObject> viewInfoObject;
    std::vector<std::weak_ptr<SimulationObject>> selectedObjects;
    std::weak_ptr<Chunk> selectedChunk;

    Logger logger;
public:
    // This property must be first
    const int unit;

    ChunkManager chunkManager;

    
    const int maxSeeDistance;
    const int allowedClickError = 10;



    Simulation(int numberOfChunksX_, int numberOfChunksY_, int unit_ = 10)
        : unit(unit_),
        chunkManager(numberOfChunksX_, numberOfChunksY_, float(unit * 10)),
        maxSeeDistance(chunkManager.chunkSize * 0.95)
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
    /// @param window_pos Position of window to draw on. Must add it to objects position 
    void render(ImDrawList *draw_list, ImVec2 window_pos, ImVec2 window_size, bool drawDebugLayer=true)
    {
        chunkManager.drawChunksMesh(draw_list, window_pos);

        ImVec2 mouse_pos = ImGui::GetMousePos();
        // Holder variable for distance between click position and object
        float dist_sq = 0.0f;
        ImVec2 object_center;
        // In future, when we will have camera will be able to move it
            
        bool isMouseClicked = ImGui::IsMouseClicked(0);
        // Check if click was inside simulation window. If not, then not count it as a click
        if (mouse_pos.x < window_pos.x || mouse_pos.y < 0 ||
            mouse_pos.x >= window_pos.x + window_size.x - 10 || mouse_pos.y >= window_pos.y + window_size.y - 10)
            { isMouseClicked = false; }
        // Click handling
        bool wasSelectedObject = false;
        if (isMouseClicked) {
            // If mouse were clicked clear all previous selections
            selectedObjects.clear();
            selectedChunk.reset();
            std::shared_ptr<Chunk> clickedChunk = chunkManager.whatChunkHere(Vec2<float>(mouse_pos) - Vec2<float>(window_pos));
            if (clickedChunk) {
                for (auto& obj : clickedChunk->objects) {
                    if (auto validObj = obj.lock()) {
                        object_center = ImVec2(window_pos.x + validObj->pos.x, window_pos.y + validObj->pos.y);
                        dist_sq = (mouse_pos.x - object_center.x) * (mouse_pos.x - object_center.x) + 
                                    (mouse_pos.y - object_center.y) * (mouse_pos.y - object_center.y);
                        if (dist_sq <= (validObj->getRadius() + allowedClickError) * (validObj->getRadius() + allowedClickError)) {
                            // setViewInfoObject(validObj);
                            selectedObjects.push_back(validObj);
                            wasSelectedObject = true;
                        }
                    }
                }
                // if chunk was clicked, but no specific object was selected
                if (!wasSelectedObject) {
                    selectedChunk = clickedChunk;
                    for (auto& obj : clickedChunk->objects) {
                        if (auto validObj = obj.lock()) {
                            selectedObjects.push_back(validObj);
                        }
                    }
                }
            }
        }
        // If chunk is selected, draw it before anything else
        if (auto validSelectedChunk = selectedChunk.lock()) {
            draw_list->AddRect(toImVec2(Vec2<float>(window_pos) + validSelectedChunk->startPos),
                                    toImVec2(Vec2<float>(window_pos) + validSelectedChunk->endPos), ImColor(colorInt(255, 255, 0, 50)), 0, 0, 2);
        }
        
        // Draw all objects by chunks
        for (auto chunk : chunkManager) {
            for (auto& obj : chunk->objects) {
                if (auto validObj = obj.lock()) {
                    validObj->draw(draw_list, window_pos);
                    
                }
            }
        }
        
        // Draw debug layer
        if (drawDebugLayer) {
            for (auto& obj : selectedObjects)
            {
                if (auto validSelectedObject = obj.lock()) {
                    validSelectedObject->drawHighlightion(draw_list, window_pos);
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
            // No need for "obj->setChunk(objectsChunk)" because Chunk::addObject() do it
        }
        else {
            throw std::invalid_argument("No chunk found for the given position. Pos: " + obj->pos.text());
        }

        objects.push_back(obj);
        return obj.get();
    }

    /// @brief Retrieves the current info view object.
    std::shared_ptr<SimulationObject> getInfoViewObject() {
        if (!selectedObjects.empty() && selectedChunk.expired()) {
            if (auto validSelectedObject = selectedObjects[selectedObjects.size() - 1].lock()) {
                return validSelectedObject;  // Lock the weak pointer to get the shared pointer
            }
        }
        return nullptr;
    }
    /// @brief Retrieves the current selected chunk.
    std::shared_ptr<Chunk> getSelectedChunk() {
        if (auto validSelectedChunk = selectedChunk.lock()) {
            return validSelectedChunk;  // Lock the weak pointer to get the shared pointer
        }
        return nullptr;
    }

    int getNumberOfObjects() { return objects.size(); }

    /// @brief Draw logger display inside given window
    /// @param parentWindowTitle Title of logger parent window
    void drawLogger(const char* parentWindowTitle) {
        logger.Draw(parentWindowTitle);
    }

    /// @brief  Output log to simulation logger window
    /// @example log(Logger::LOG, "Radius: %i \n ObjectInVision: %i\n", getSeeDistance(), objectsInVision.size());
    /// @param logType Type of log: LOG, WARNING, ERROR
    /// @param fmt Formating string 
    /// @param `...` Args for formating string
    void log(Logger::LogType logType, const char* fmt, ...) {
        const char* prefix = "";
        switch (logType) {
            case Logger::LOG:     prefix = "(LOG) "; break;
            case Logger::WARNING: prefix = "[WARNING] "; break;
            case Logger::ERROR:   prefix = "<ERROR> "; break;
            default:              prefix = ".UNKNOWN. "; break;
        }

        // Create a buffer for the full message (prefix + formatted message)
        char formattedMessage[512]; // Adjust size as needed

        // Format the variadic arguments into a separate buffer
        char messageBody[512];
        va_list args;
        va_start(args, fmt);
        vsnprintf(messageBody, sizeof(messageBody), fmt, args);
        va_end(args);

        // Combine the prefix with the formatted message
        snprintf(formattedMessage, sizeof(formattedMessage), "%s%s", prefix, messageBody);

        // Call AddLog with the combined message
        logger.AddLog("%s", formattedMessage);
    }

};
