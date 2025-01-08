#pragma once

#include "simulation.h"
#include "simulation_objects.h"
#include "simulation_bots.h"

#include "imgui.h"
#include "imgui_internal.h"

/// @brief Create gui of simulation to draw on frame
void createGui(std::shared_ptr<Simulation> simulation, ImGuiIO& io);

/// @brief Create simulation objects list for preview and managment
void createObjectListGui(std::shared_ptr<Simulation> simulation);

/// @brief Check for specific keys pressed and make selected bot do according action
void handleBotKeysEvent(std::shared_ptr<Simulation> simulation);
