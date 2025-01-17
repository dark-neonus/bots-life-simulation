#pragma once

#include "EvolutionPointsSettings.h"
#include "SimulationSizeSettings.h"
#include "MapGenerationSettings.h"

class SimulationSettings {
public:
    EvolutionPointsSettings evolutionPointsSettings;
    SimulationSizeSettings simulationSizeSettings;
    MapGenerationSettings mapGenerationSettings;

    bool drawGui = false;

    SimulationSettings() = default;
};