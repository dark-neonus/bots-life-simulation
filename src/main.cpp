#include <memory>
#include <random>

#include "settings/SimulationSettings.h"
#include "simulation.h"
#include "gui/guiLoop.h"
#include "protocols/brain/BotBrain.h"
#include "protocols/brain/examples/Dummy.h"
#include "protocols/brain/examples/Survival.h"

int main()
{
    std::shared_ptr<SimulationSettings> settings = std::make_shared<SimulationSettings>();

    settings->simulationSizeSettings.unit = 50;
    settings->simulationSizeSettings.numberOfChunksX = 40;
    settings->simulationSizeSettings.numberOfChunksY = 40;

    std::shared_ptr<Simulation> simulation = std::make_shared<Simulation>(
        std::const_pointer_cast<const SimulationSettings>(settings)
        );


    simulation->addSmartBot(
        std::dynamic_pointer_cast<BotBrain>(
            std::make_shared<DummyBotBrain>()
        ),
        Vec2<float>(150.0f, 150.0f)
    );

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<int> distX = std::uniform_int_distribution<int>(0, simulation->chunkManager->mapWidth - 1);
    std::uniform_int_distribution<int> distY = std::uniform_int_distribution<int>(0, simulation->chunkManager->mapHeight - 1);

    for (int i = 0; i < 1000; i++) {
        simulation->addSmartBot(
            std::dynamic_pointer_cast<BotBrain>(
                std::make_shared<SurvivalBotBrain>()
            ),
            Vec2<float>(distX(gen), distY(gen))
        );
    }

    guiLoop(simulation);

    return 0;
}