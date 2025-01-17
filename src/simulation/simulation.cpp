#include "simulation.h"

#include <tuple>
#include <random>
#include <memory>

#include "objects/SimulationObject.h"
#include "objects/Food.h"
#include "objects/Tree.h"
#include "objects/Bot.h"
#include "gui/gui.h"

#include "protocols/brain/BotBrain.h"

#include "settings/SimulationSettings.h"
#include "protocols/brain/BrainsRegistry.h"


Simulation::Simulation(std::shared_ptr<const SimulationSettings> settings_)
    : unit(settings_->simulationSizeSettings.unit),
      chunkManager(
          std::make_unique<ChunkManager>(
              settings_->simulationSizeSettings.numberOfChunksX,
              settings_->simulationSizeSettings.numberOfChunksY,
              float(settings_->simulationSizeSettings.unitsPerChunk * settings_->simulationSizeSettings.unit))),
      maxSeeDistance(chunkManager->chunkSize * settings_->evolutionPointsSettings.maxSeeDistanceSizeOfChunk),
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
        if (obj)
        {
            obj->onDestroy();
            // log(Logger::LOG, "Object [%0*lu] deletion process started\n", 6, obj->id.get());

            if (auto chunk = obj->getChunk())
            {
                chunk->removeObject(obj);
            }

            auto it = std::find(objects.begin(), objects.end(), obj);
            if (it != objects.end())
            {
                objects.erase(it);
            }
        }
        deathNote.pop();
        // log(Logger::LOG, "Object deleted successfully!\n");
    }
    while (!bornQueue.empty()) {
        auto& bornArgs = bornQueue.front();
        addSmartBot(std::get<0>(bornArgs), std::get<1>(bornArgs), 0.1f, 0.5f, std::get<2>(bornArgs));
        bornQueue.pop();
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
    ImVec2 mouse_map_pos = ImVec2(window_pos.x + (mouse_pos.x - window_pos.x + camera.x()) / camera.zoom.get(),
                                    window_pos.y + (mouse_pos.y - window_pos.y + camera.y()) / camera.zoom.get());
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
    chunkManager->drawChunksMesh(draw_list, drawing_delta_pos, camera.zoom.get());

    // If chunk is selected, draw it before anything else
    if (auto validSelectedChunk = selectedChunk.lock())
    {
        draw_list->AddRect(toImVec2(Vec2<float>(drawing_delta_pos) + validSelectedChunk->startPos * camera.zoom.get()),
                           toImVec2(Vec2<float>(drawing_delta_pos) + validSelectedChunk->endPos * camera.zoom.get()), ImColor(colorInt(255, 255, 0, 50)), 0, 0, 2);
    }

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
                    validObj->draw(draw_list, drawing_delta_pos, camera.zoom.get());
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
                validSelectedObject->drawHighlightion(draw_list, drawing_delta_pos, camera.zoom.get());
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

    obj->setID(simulation->idManger.getAssignValue());
    

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

std::shared_ptr<BotObject> Simulation::addSmartBot(std::shared_ptr<BotBrain> brain,
                                                   Vec2<float> pos,
                                                   float startingHealthKoef,
                                                   float startingFoodKoef,
                                                   int evolutionPoints)
{
    brain->protocolsHolder->initProtocol.botSpawnPosition = pos;
    brain->protocolsHolder->initProtocol.evolutionPoints = evolutionPoints == -1 ? settings->evolutionPointsSettings.amountOfPoints : evolutionPoints;
    brain->init(brain->protocolsHolder->initProtocol, brain->protocolsHolder->initProtocolResponce);

    brain->protocolsHolder->initProtocolResponce.healthPoints = std::max(0, brain->protocolsHolder->initProtocolResponce.healthPoints);
    brain->protocolsHolder->initProtocolResponce.foodPoints = std::max(0, brain->protocolsHolder->initProtocolResponce.foodPoints);
    brain->protocolsHolder->initProtocolResponce.visionPoints = std::max(0, brain->protocolsHolder->initProtocolResponce.visionPoints);
    brain->protocolsHolder->initProtocolResponce.speedPoints = std::max(0, brain->protocolsHolder->initProtocolResponce.speedPoints);
    brain->protocolsHolder->initProtocolResponce.attackPoints = std::max(0, brain->protocolsHolder->initProtocolResponce.attackPoints);

    // TODO: change throw logic to cutting points
    if (
        brain->protocolsHolder->initProtocolResponce.healthPoints +
            brain->protocolsHolder->initProtocolResponce.foodPoints +
            brain->protocolsHolder->initProtocolResponce.visionPoints +
            brain->protocolsHolder->initProtocolResponce.speedPoints +
            brain->protocolsHolder->initProtocolResponce.attackPoints >
        brain->protocolsHolder->initProtocol.evolutionPoints)
    {
        throw std::invalid_argument("You spent more than maximum evolution points!");
    }

    startingHealthKoef = std::clamp(startingHealthKoef, 0.0f, 1.0f);
    startingFoodKoef = std::clamp(startingFoodKoef, 0.0f, 1.0f);

    std::shared_ptr<BotObject> bot = std::make_shared<BotObject>(
        shared_from_this(),
        pos,
        settings->evolutionPointsSettings.PointsToHealth( // Health value
            brain->protocolsHolder->initProtocolResponce.healthPoints) * startingHealthKoef,
        settings->evolutionPointsSettings.PointsToFood( // Food value
            brain->protocolsHolder->initProtocolResponce.foodPoints) * startingFoodKoef,
        std::min(
            settings->evolutionPointsSettings.PointsToVisionDistance( // See distance value
                brain->protocolsHolder->initProtocolResponce.visionPoints),
            maxSeeDistance),
        settings->evolutionPointsSettings.PointsToSpeed( // Speed value
            brain->protocolsHolder->initProtocolResponce.speedPoints),
        settings->evolutionPointsSettings.PointsToDamage( // Damage value
            brain->protocolsHolder->initProtocolResponce.attackPoints),
        settings->evolutionPointsSettings.PointsToHealth( // Max health value
            brain->protocolsHolder->initProtocolResponce.healthPoints),
        settings->evolutionPointsSettings.PointsToFood( // Max food value
            brain->protocolsHolder->initProtocolResponce.foodPoints)
        );

    bot->setColor(ImColor(colorInt(
        std::max(0, std::min(brain->protocolsHolder->initProtocolResponce.r, 255)),
        std::max(0, std::min(brain->protocolsHolder->initProtocolResponce.g, 255)),
        std::max(0, std::min(brain->protocolsHolder->initProtocolResponce.b, 255)),
        255)));

    bot->setBrainObject(brain);

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

    bot->setID(idManger.getAssignValue());

    rawAddToObjectList(bot);
    return bot;
}

void Simulation::initBotClasses() {
    if (!settings) {
        log(Logger::ERROR, "Simulation settings are not initialized.");
        return;
    }

    const auto& mapSettings = settings->mapGenerationSettings;
    std::random_device rd;
    std::mt19937 gen(rd());

    // Calculate the map center
    Vec2<float> mapCenter(
        static_cast<float>(chunkManager->mapWidth) / 2.0f,
        static_cast<float>(chunkManager->mapHeight) / 2.0f
    );

    // Set spawn radius for circle type to 75% of the smallest map dimension
    float circleRadius = 0.30f * std::min(chunkManager->mapWidth, chunkManager->mapHeight);

    // Distributions for random coordinates
    std::uniform_int_distribution<int> distX(0, chunkManager->mapWidth - 1);
    std::uniform_int_distribution<int> distY(0, chunkManager->mapHeight - 1);

    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * M_PI);
    std::uniform_real_distribution<float> radiusDist(0.0f, mapSettings.spawnRadius);  // Radius for random spawn

    // Lambda to clamp positions to stay within map bounds
    auto clampPosition = [&](const Vec2<float>& pos) {
        return Vec2<float>(
            std::clamp(pos.x, 0.0f, static_cast<float>(chunkManager->mapWidth - 1)),
            std::clamp(pos.y, 0.0f, static_cast<float>(chunkManager->mapHeight - 1))
        );
        };

    auto botNames = BrainsRegistry::getInstance().listRegisteredBots();

    for (const auto& name : botNames) {
        // Only generate random values if spawn type is Random
        for (unsigned int i = 0; i < mapSettings.numberOfBotsPerPopulation; ++i) {
            Vec2<float> spawnPos;

            // Start with the correct spawn type logic
            switch (mapSettings.spawnType) {
            case SpawnType::Random: {
                spawnPos = Vec2<float>(distX(gen), distY(gen));
                break;
            }

            case SpawnType::Circle: {
                float angle = angleDist(gen);
                spawnPos = Vec2<float>(mapCenter.x + circleRadius * std::cos(angle),
                    mapCenter.y + circleRadius * std::sin(angle));
                break;
            }

            case SpawnType::OnePlace:
                // Spawn all bots in one central location (e.g., map center)
                spawnPos = mapCenter;
                break;

            default:
                log(Logger::WARNING, "Unknown spawn type. Defaulting to random.");
                spawnPos = Vec2<float>(distX(gen), distY(gen));
                break;
            }

            // Add additional random offset using spawnRadius
            float angle = angleDist(gen);
            float radius = radiusDist(gen);
            spawnPos += {radius * std::cos(angle), radius * std::sin(angle)};

            // Clamp the final spawn position to ensure it's within the map bounds
            spawnPos = clampPosition(spawnPos);

            // Create and add the bot to the simulation
            addSmartBot(
                BrainsRegistry::getInstance().createBot(name),
                spawnPos
            );
        }
    }
}

