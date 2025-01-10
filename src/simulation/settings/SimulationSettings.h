#pragma once

#include "EvolutionPointsSettings.h"
#include "SimulationSizeSettings.h"

class SimulationSettings {
public:
    EvolutionPointsSettings evolutionPointsSettings;
    SimulationSizeSettings simulationSizeSettings;
    bool drawGui = false;

    SimulationSettings() = default;
};