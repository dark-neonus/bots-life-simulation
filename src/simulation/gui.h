#pragma once

#include "simulation.h"
#include "simulation_objects.h"
#include "simulation_bots.h"

#include "imgui.h"
#include "imgui_internal.h"

/// @brief Create gui of simulation to draw on frame
void CreateGui(std::shared_ptr<Simulation> simulation, ImGuiIO& io);

/// @brief Create simulation objects list for preview and managment
void CreateObjectListGui(std::shared_ptr<Simulation> simulation);