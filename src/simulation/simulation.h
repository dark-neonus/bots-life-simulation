#pragma once

#include <vector>
#include <queue>
#include <stdexcept>
#include <algorithm>
#include <memory>
#include <tuple>

#include "imgui.h"
#include "utilities/utilities.h"
#include "chunks.h"
#include "objects/SimulationObject.h"
#include "settings/SimulationSettings.h"
// #include "protocols/brain/BrainsRegistry.h"

#ifndef SIMULATION_OBJECT_TYPE_ENUM
#define SIMULATION_OBJECT_TYPE_ENUM
enum class SimulationObjectType {
    BaseObject,
    BotObject,
    FoodObject,
    TreeObject,
};
#endif

class IDManager;
class Camera;
class ChunkManager;

class SimulationObject;
class FoodObject;
class TreeObject;
class BotObject;
class BotBrain;

class Simulation;

class Simulation : public std::enable_shared_from_this<Simulation>
{
private:
    std::vector<std::shared_ptr<SimulationObject>> objects;

    // std::weak_ptr<SimulationObject> viewInfoObject;
    std::vector<std::weak_ptr<SimulationObject>> selectedObjects;
    std::weak_ptr<Chunk> selectedChunk;

    Logger logger;

    // Queue of all object that will be deleted in Simulation::afterUpdate() after Simulation::update()
    std::queue<std::shared_ptr<SimulationObject>> deathNote;

    std::queue<std::tuple<std::shared_ptr<BotBrain>, Vec2<float>, int>> bornQueue;
public:
    IDManager idManger;
    // This property must be first
    const int unit;

    std::unique_ptr<ChunkManager> chunkManager;

    const int maxSeeDistance;
    const int allowedClickError = 10;

    Camera camera;

    std::shared_ptr<const SimulationSettings> settings;


    Simulation(std::shared_ptr<const SimulationSettings> settings_);

    /// @brief Update all objects in simulation
    void update(bool isSimulationRunning);

    /// @brief Adds a SimulationObject to the death note queue for deletion after the update.
    void addToDeathNote(std::shared_ptr<SimulationObject> obj)
    {
        this->deathNote.push(obj);
    }

    /// @brief Function to call after Simulation::update(). For now just delete objects in Simulation::deathNote
    void afterUpdate();

    /// @brief Render all objects in simulation
    /// @param draw_list Object to draw on provided by ImGui
    /// @param window_pos Position of window to draw on. Must add it to objects position
    void render(ImDrawList *draw_list, ImVec2 window_pos, ImVec2 window_size, bool drawDebugLayer = true); // definition in simulation.cpp
    
    void addObject(SimulationObjectType type, std::shared_ptr<SimulationObject> obj);

    void selectSingleObject(std::shared_ptr<SimulationObject> objectToSelect);

    /// @brief Retrieves the current info view object.
    std::shared_ptr<SimulationObject> getSelectedObject()
    {
        if (!selectedObjects.empty() && selectedChunk.expired())
        {
            if (auto validSelectedObject = selectedObjects[selectedObjects.size() - 1].lock())
            {
                return validSelectedObject; // Lock the weak pointer to get the shared pointer
            }
        }
        return nullptr;
    }
    /// @brief Retrieves the current selected chunk.
    std::shared_ptr<Chunk> getSelectedChunk()
    {
        if (auto validSelectedChunk = selectedChunk.lock())
        {
            return validSelectedChunk; // Lock the weak pointer to get the shared pointer
        }
        return nullptr;
    }

    int getNumberOfObjects() { return objects.size(); }

    /// @brief Draw logger display inside given window
    /// @param parentWindowTitle Title of logger parent window
    void drawLogger(const char *parentWindowTitle)
    {
        logger.Draw(parentWindowTitle);
    }

    /// @brief  Output log to simulation logger window
    /// @example log(Logger::LOG, "Radius: %i \n ObjectInVision: %i\n", getSeeDistance(), objectsInVision.size());
    /// @param logType Type of log: LOG, WARNING, ERROR
    /// @param fmt Formating string
    /// @param `...` Args for formating string
    void log(Logger::LogType logType, const char *fmt, ...);

    std::shared_ptr<std::vector<std::shared_ptr<SimulationObject>>> getObjects()
    {
        return std::make_shared<std::vector<std::shared_ptr<SimulationObject>>>(objects);
    }

    void rawAddToObjectList(std::shared_ptr<SimulationObject> obj) {
        objects.push_back(obj);
    }

    /// @brief Create bot object in simulation with the given brain
    /// @param brain Brain of new bot
    std::shared_ptr<BotObject> addSmartBot(std::shared_ptr<BotBrain> brain,
                                                   Vec2<float> pos,
                                                   float startingHealthKoef = 1.0f,
                                                   float startingFoodKoef = 1.0f,
                                                   int evolutionPoints = -1);

    void addBotToBorn(std::tuple<std::shared_ptr<BotBrain>, Vec2<float>, int> bornArgs) {
        bornQueue.push(bornArgs);
    }

    void initBotClasses();
};
