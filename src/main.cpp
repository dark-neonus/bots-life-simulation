#include <memory>

#include "settings/SimulationSettings.h"
#include "simulation.h"
#include "gui/guiLoop.h"
#include "protocols/brain/BotBrain.h"
#include "protocols/brain/examples/Dummy.h"

int main()
{
    std::shared_ptr<SimulationSettings> settings = std::make_shared<SimulationSettings>();

    std::shared_ptr<Simulation> simulation = std::make_shared<Simulation>(
        std::const_pointer_cast<const SimulationSettings>(settings)
        );


    simulation->addSmartBot(
        std::dynamic_pointer_cast<BotBrain>(
            std::make_shared<DummyBotBrain>()
        ),
        Vec2<float>(150.0f, 150.0f)
    );

    guiLoop(simulation);

    return 0;
}