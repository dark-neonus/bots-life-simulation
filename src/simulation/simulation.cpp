#include "simulation.h"
#include "objects/SimulationObject.h"
#include "objects/Food.h"
#include "objects/Tree.h"
#include "objects/Bot.h"
#include "gui/gui.h"

#include "protocols/brain/BotBrain.h"

#include "settings/SimulationSettings.h"

Simulation::Simulation(std::shared_ptr<const SimulationSettings> settings_)
    : unit(settings_->simulationSizeSettings.unit),
      chunkManager(
        std::make_unique<ChunkManager>(
            settings_->simulationSizeSettings.numberOfChunksX,
            settings_->simulationSizeSettings.numberOfChunksY,
            float(settings_->simulationSizeSettings.unitsPerChunk * settings_->simulationSizeSettings.unit)
            )
        ),
      maxSeeDistance(chunkManager->chunkSize * settings_->evolutionPointsSettings.VisionDistanceForPoint),
      camera(float(chunkManager->mapWidth), float(chunkManager->mapHeight)),
      settings(settings_)
{
}

void Simulation::update(bool isSimulationRunning)
{
    if (!isSimulationRunning)
    {
        return;
    }

    std::vector<std::shared_ptr<SimulationObject>> objects_to_update = objects;

    for (auto &obj : objects_to_update)
    {
        if (obj != nullptr)
        {
            obj->update();
        }
    }
}

void Simulation::afterUpdate()
{
    while (!deathNote.empty())
    {
        auto &obj = deathNote.front();
        obj->onDestroy();
        // log(Logger::LOG, "Object [%0*lu] deletion process started\n", 6, obj->id.get());
        deathNote.pop();

        if (auto chunk = obj->getChunk())
        {
            chunk->removeObject(obj);
        }

        auto it = std::find(objects.begin(), objects.end(), obj);
        if (it != objects.end())
        {
            objects.erase(it);
        }
        // log(Logger::LOG, "Object deleted successfully!\n");
    }
}

void Simulation::render(ImDrawList *draw_list, ImVec2 window_pos, ImVec2 window_size, bool drawDebugLayer)
{

    camera.setSize(window_size.x - 20, window_size.y - 40);
    camera.update();
    ImVec2 drawing_delta_pos = ImVec2(window_pos.x - camera.x(), window_pos.y - camera.y());

    auto cameraStartPos = camera.getTopLeft();
    auto cameraEndPos = camera.getBottomRight();

    int startChunkX = std::max(0, int(cameraStartPos.x / chunkManager->chunkSize - 1));
    int startChunkY = std::max(0, int(cameraStartPos.y / chunkManager->chunkSize - 1));

    int endChunkX = std::min(chunkManager->numberOfChunksX - 1, int(cameraEndPos.x / chunkManager->chunkSize + 1));
    int endChunkY = std::min(chunkManager->numberOfChunksY - 1, int(cameraEndPos.y / chunkManager->chunkSize + 1));

    ImVec2 mouse_pos = ImGui::GetMousePos();
    ImVec2 mouse_map_pos = ImVec2(mouse_pos.x + camera.x(), mouse_pos.y + camera.y());
    // Holder variable for distance between click position and object
    float dist_sq = 0.0f;
    ImVec2 object_center;
    // In future, when we will have camera will be able to move it

    bool isMouseClicked = ImGui::IsMouseClicked(0);
    // Check if click was inside simulation window. If not, then not count it as a click
    if (mouse_pos.x < window_pos.x || mouse_pos.y < 0 ||
        mouse_pos.x >= window_pos.x + window_size.x - 10 || mouse_pos.y >= window_pos.y + window_size.y - 10)
    {
        isMouseClicked = false;
    }

    // Click handling
    bool wasSelectedObject = false;
    if (isMouseClicked)
    {
        // If mouse were clicked clear all previous selections
        selectedObjects.clear();
        selectedChunk.reset();
        std::shared_ptr<Chunk> clickedChunk = chunkManager->whatChunkHere(Vec2<float>(mouse_map_pos) - Vec2<float>(window_pos));
        if (clickedChunk)
        {
            for (auto &obj : clickedChunk->objects)
            {
                if (auto validObj = obj.lock())
                {
                    object_center = ImVec2(window_pos.x + validObj->pos.x, window_pos.y + validObj->pos.y);
                    dist_sq = (mouse_map_pos.x - object_center.x) * (mouse_map_pos.x - object_center.x) +
                              (mouse_map_pos.y - object_center.y) * (mouse_map_pos.y - object_center.y);
                    if (dist_sq <= (validObj->getRadius() + allowedClickError) * (validObj->getRadius() + allowedClickError))
                    {
                        // setViewInfoObject(validObj);
                        selectedObjects.push_back(validObj);
                        wasSelectedObject = true;
                    }
                }
            }
            // if chunk was clicked, but no specific object was selected
            if (!wasSelectedObject)
            {
                selectedChunk = clickedChunk;
                for (auto &obj : clickedChunk->objects)
                {
                    if (auto validObj = obj.lock())
                    {
                        selectedObjects.push_back(validObj);
                    }
                }
            }
        }
    }
    // Draw map mesh
    chunkManager->drawChunksMesh(draw_list, drawing_delta_pos);

    // If chunk is selected, draw it before anything else
    if (auto validSelectedChunk = selectedChunk.lock())
    {
        draw_list->AddRect(toImVec2(Vec2<float>(drawing_delta_pos) + validSelectedChunk->startPos),
                           toImVec2(Vec2<float>(drawing_delta_pos) + validSelectedChunk->endPos), ImColor(colorInt(255, 255, 0, 50)), 0, 0, 2);
    }

    // Draw all objects by chunks
    // for (auto chunk : *chunkManager.get()) {
    //     for (auto& obj : chunk->objects) {
    //         if (auto validObj = obj.lock()) {
    //             validObj->draw(draw_list, drawing_delta_pos);
    //
    //         }
    //     }
    // }

    // Draw objects within visible chunks
    for (int chunkY = startChunkY; chunkY <= endChunkY; ++chunkY)
    {
        for (int chunkX = startChunkX; chunkX <= endChunkX; ++chunkX)
        {
            auto chunk = chunkManager->getChunk(chunkX, chunkY);
            for (auto &obj : chunk->objects)
            {
                if (auto validObj = obj.lock())
                {
                    validObj->draw(draw_list, drawing_delta_pos);
                }
            }
        }
    }

    // Draw debug layer
    if (drawDebugLayer)
    {
        for (auto &obj : selectedObjects)
        {
            if (auto validSelectedObject = obj.lock())
            {
                validSelectedObject->drawHighlightion(draw_list, drawing_delta_pos);
            }
        }
    }
}

void Simulation::selectSingleObject(std::shared_ptr<SimulationObject> objectToSelect)
{
    selectedObjects.clear();
    selectedObjects.push_back(objectToSelect);
    selectedChunk.reset();
    if (!camera.isPointInVision(objectToSelect->pos))
    {
        camera.moveTo(objectToSelect->pos);
    }
}

template <typename T, typename... Args>
T *addObjectToSimulation(std::shared_ptr<Simulation> simulation, std::shared_ptr<T> obj_)
{
    std::shared_ptr<T> obj = std::make_shared<T>(*obj_);

    // Check if position is valid (inside map)
    if (obj->pos.x < 0 || obj->pos.y < 0 ||
        obj->pos.x > simulation->chunkManager->mapWidth || obj->pos.y > simulation->chunkManager->mapHeight)
    {
        throw std::invalid_argument("Position of object is out of simualtion map. Pos: " + obj->pos.text());
    }

    // Assign chunk to object and object to chunk
    std::shared_ptr<Chunk> objectsChunk = simulation->chunkManager->whatChunkHere(obj->pos);
    if (objectsChunk)
    {
        objectsChunk->addObject<T>(obj);
        // No need for "obj->setChunk(objectsChunk)" because Chunk::addObject() do it
    }
    else
    {
        throw std::invalid_argument("No chunk found for the given position. Pos: " + obj->pos.text());
    }

    obj->id.set(simulation->idManger.getAssignValue());

    simulation->rawAddToObjectList(obj);
    return obj.get();
}

void Simulation::addObject(SimulationObjectType objectType, std::shared_ptr<SimulationObject> obj)
{
    switch (objectType)
    {
    case SimulationObjectType::BaseObject:
        addObjectToSimulation<SimulationObject>(shared_from_this(), std::dynamic_pointer_cast<SimulationObject>(obj));
        break;
    case SimulationObjectType::FoodObject:
        addObjectToSimulation<FoodObject>(shared_from_this(), std::dynamic_pointer_cast<FoodObject>(obj));
        break;
    case SimulationObjectType::TreeObject:
        addObjectToSimulation<TreeObject>(shared_from_this(), std::dynamic_pointer_cast<TreeObject>(obj));
        break;
    case SimulationObjectType::BotObject:
        addObjectToSimulation<BotObject>(shared_from_this(), std::dynamic_pointer_cast<BotObject>(obj));
        break;
    default:
        std::runtime_error("Invalid object type!");
    }
}

void Simulation::log(Logger::LogType logType, const char *fmt, ...)
{
    const char *prefix = "";
    switch (logType)
    {
    case Logger::LOG:
        prefix = "(LOG) ";
        break;
    case Logger::WARNING:
        prefix = "[WARNING] ";
        break;
    case Logger::ERROR:
        prefix = "<ERROR> ";
        break;
    default:
        prefix = ".UNKNOWN. ";
        break;
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
    snprintf(formattedMessage, sizeof(prefix) + sizeof(formattedMessage), "%s%s", prefix, messageBody);

    // Call AddLog with the combined message
    logger.AddLog("%s", formattedMessage);
}

std::shared_ptr<BotObject> Simulation::addSmartBot(std::shared_ptr<BotBrain> brain, Vec2<float> pos)
{
    brain->protocolsHolder->initProtocol.botSpawnPosition = pos;
    brain->protocolsHolder->initProtocol.evolutionPoints = 100;
    brain->init();

    std::shared_ptr<BotObject> bot = std::make_shared<BotObject>(
        shared_from_this(),
        pos,
        settings->evolutionPointsSettings.PointsToHealth(
            brain->protocolsHolder->initProtocolResponce.healthPoints
        ),
        settings->evolutionPointsSettings.PointsToFood(
            brain->protocolsHolder->initProtocolResponce.foodPoints
        ),
        settings->evolutionPointsSettings.PointsToVisionDistance(
            brain->protocolsHolder->initProtocolResponce.visionPoints
        ),
        settings->evolutionPointsSettings.PointsToSpeed(
            brain->protocolsHolder->initProtocolResponce.speedPoints
        ),
        settings->evolutionPointsSettings.PointsToDamage(
            brain->protocolsHolder->initProtocolResponce.attackPoints
        )
    );

    // Check if position is valid (inside map)
    if (bot->pos.x < 0 || bot->pos.y < 0 ||
        bot->pos.x > chunkManager->mapWidth || bot->pos.y > chunkManager->mapHeight)
    {
        throw std::invalid_argument("Position of bot is out of simualtion map. Pos: " + bot->pos.text());
    }

    // Assign chunk to object and object to chunk
    std::shared_ptr<Chunk> objectsChunk = chunkManager->whatChunkHere(bot->pos);
    if (objectsChunk)
    {
        objectsChunk->addObject<BotObject>(bot);
        // No need for "obj->setChunk(objectsChunk)" because Chunk::addObject() do it
    }
    else
    {
        throw std::invalid_argument("No chunk found for the given position. Pos: " + bot->pos.text());
    }

    bot->id.set(idManger.getAssignValue());

    rawAddToObjectList(bot);
    return bot;
}