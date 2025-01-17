#include <memory>

#include "settings/SimulationSettings.h"
#include "simulation.h"
#include "objects/Tree.h"
#include "gui/guiLoop.h"
#include "protocols/brain/BotBrain.h"
#include "protocols/brain/examples/Dummy.h"
#include "protocols/brain/examples/Survival.h"
// #include "protocols/brain/examples/Multiplier.h"
#include "protocols/brain/examples/Evolutioner.h"
#include "protocols/brain/examples/AggressiveMultiplier.h"
#include "protocols/brain/examples/AggressiveMultiplier2.h"

int main()
{
    std::shared_ptr<SimulationSettings> settings = std::make_shared<SimulationSettings>();

    settings->simulationSizeSettings.unit = 50;
    settings->simulationSizeSettings.numberOfChunksX = 20;
    settings->simulationSizeSettings.numberOfChunksY = 20;

    settings->mapGenerationSettings.spawnType = SpawnType::Circle;
    settings->mapGenerationSettings.numberOfBotsPerPopulation = 70;
    settings->mapGenerationSettings.spawnRadius = 50.0f;
    settings->mapGenerationSettings.numberOfTreePerChunk = 3.5f;
    settings->mapGenerationSettings.randomSpawnFood = true;
    settings->mapGenerationSettings.foodPerChunk = 2.5f;
    settings->mapGenerationSettings.foodSpawnChance = 0.1f;

    std::shared_ptr<Simulation> simulation = std::make_shared<Simulation>(
        std::const_pointer_cast<const SimulationSettings>(settings)
        );


    // simulation->addSmartBot(
    //     std::dynamic_pointer_cast<BotBrain>(
    //         std::make_shared<DummyBotBrain>()
    //     ),
    //     Vec2<float>(150.0f, 150.0f)
    // );

    simulation->initBotClasses();

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<int> distX = std::uniform_int_distribution<int>(100, simulation->chunkManager->mapWidth - 100);
    std::uniform_int_distribution<int> distY = std::uniform_int_distribution<int>(100, simulation->chunkManager->mapHeight - 100);
    std::uniform_int_distribution<int> fruitAmountRandom = std::uniform_int_distribution<int>(3, 4);
    std::uniform_int_distribution<int> maxCalories = std::uniform_int_distribution<int>(50, 200);
    std::uniform_int_distribution<int> respawnTime = std::uniform_int_distribution<int>(250, 600);

    for (int i = 0; i < simulation->chunkManager->numberOfChunksX * simulation->chunkManager->numberOfChunksY * 2; i++) {
        simulation->addObject(SimulationObjectType::TreeObject,
            std::dynamic_pointer_cast<SimulationObject> (
                std::make_shared<TreeObject>(
                    simulation,
                    Vec2<float>(distX(gen), distY(gen)),
                    fruitAmountRandom(gen),
                    maxCalories(gen),
                    0.5f,
                    1.5f,
                    respawnTime(gen),
                    false
                )
            )
        );
    }

    // for (int i = 0; i < 1000; i++) {
    //     simulation->addSmartBot(
    //         std::dynamic_pointer_cast<BotBrain>(
    //             std::make_shared<SurvivalBotBrain>()
    //         ),
    //         Vec2<float>(distX(gen), distY(gen))
    //     );
    // }
    // for (int i = 0; i < 1; i++) {
    //     simulation->addSmartBot(
    //         std::dynamic_pointer_cast<BotBrain>(
    //             std::make_shared<MultiplierBotBrain>()
    //         ),
    //         // Vec2<float>(150.0f, 150.0f)
    //         Vec2<float>(distX(gen), distY(gen))
    //     );
    // }

    guiLoop(simulation);

    return 0;
}