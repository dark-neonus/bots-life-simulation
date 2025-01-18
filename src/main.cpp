#include <memory>

#include "settings/SimulationSettings.h"
#include "simulation.h"
#include "objects/Tree.h"
#include "gui/guiLoop.h"
#include "protocols/brain/BotBrain.h"
#include "BotRegister.h"

int main()
{
    std::shared_ptr<SimulationSettings> settings = std::make_shared<SimulationSettings>();

    settings->simulationSizeSettings.unit = 50;
    settings->simulationSizeSettings.numberOfChunksX = 20;
    settings->simulationSizeSettings.numberOfChunksY = 20;

    settings->mapGenerationSettings.spawnType = SpawnType::Circle;
    settings->mapGenerationSettings.numberOfBotsPerPopulation = 70;
    settings->mapGenerationSettings.spawnRadius = 50.0f;
    // settings->mapGenerationSettings.numberOfTreePerChunk = 3.5f;
    settings->mapGenerationSettings.randomSpawnFood = true;
    settings->mapGenerationSettings.foodPerChunk = 3.0f;
    settings->mapGenerationSettings.foodSpawnChance = 0.005f;

    std::shared_ptr<Simulation> simulation = std::make_shared<Simulation>(
        std::const_pointer_cast<const SimulationSettings>(settings)
    );

    simulation->initBotClasses();
    simulation->generateTree();

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<int> distX = std::uniform_int_distribution<int>(100, simulation->chunkManager->mapWidth - 100);
    std::uniform_int_distribution<int> distY = std::uniform_int_distribution<int>(100, simulation->chunkManager->mapHeight - 100);
    std::uniform_int_distribution<int> fruitAmountRandom = std::uniform_int_distribution<int>(3, 4);
    std::uniform_int_distribution<int> maxCalories = std::uniform_int_distribution<int>(50, 200);
    std::uniform_int_distribution<int> respawnTime = std::uniform_int_distribution<int>(250, 600);

    // for (int i = 0; i < simulation->chunkManager->numberOfChunksX * simulation->chunkManager->numberOfChunksY * 2; i++) {
    //     simulation->addObject(SimulationObjectType::TreeObject,
    //         std::dynamic_pointer_cast<SimulationObject> (
    //             std::make_shared<TreeObject>(
    //                 simulation,
    //                 Vec2<float>(distX(gen), distY(gen)),
    //                 fruitAmountRandom(gen),
    //                 maxCalories(gen),
    //                 0.5f,
    //                 1.5f,
    //                 respawnTime(gen),
    //                 false
    //             )
    //         )
    //     );
    // }

    guiLoop(simulation);

    return 0;
}