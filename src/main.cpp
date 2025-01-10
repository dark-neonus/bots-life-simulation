#include <memory>

#include "settings/SimulationSettings.h"
#include "simulation.h"
#include "gui/guiLoop.h"

int main()
{
    std::shared_ptr<SimulationSettings> settings = std::make_shared<SimulationSettings>();

    std::shared_ptr<Simulation> simulation = std::make_shared<Simulation>(
        std::const_pointer_cast<const SimulationSettings>(settings)
        );

    guiLoop(simulation);

    return 0;
}