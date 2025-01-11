#include "gui.h"

#include <chrono>
#include <memory>

#include "simulation.h"
#include "objects/Bot.h"

void createGui(std::shared_ptr<Simulation> simulation, ImGuiIO& io) {
    static auto logicTimeStart = std::chrono::high_resolution_clock::now(); 
    static auto logicTimeEnd = std::chrono::high_resolution_clock::now();
    static std::chrono::duration<double, std::milli> logicTimeElapsed;
    static double logicFramerate;
    static bool isSimulationRunning = true;
    
    if (ImGui::IsKeyPressed(ImGuiKey_Space)) {
        isSimulationRunning = !isSimulationRunning;
    }

    ImGuiID dockspace_id = ImGui::DockSpaceOverViewport();
    static bool dockspaceInit = true;
    ImGuiID dock_id_simulation, dock_id_infotab, dock_id_logger;
    if (dockspaceInit) {
        dockspaceInit = false;
        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id);
        ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

        ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.7f, &dock_id_simulation, &dock_id_infotab);
        ImGui::DockBuilderDockWindow("Simulation window", dock_id_simulation);
        ImGui::DockBuilderDockWindow("Information", dock_id_infotab);

        ImGui::DockBuilderFinish(dockspace_id);

        ImGui::DockBuilderSplitNode(dock_id_simulation, ImGuiDir_Up, 0.8f, &dock_id_simulation, &dock_id_logger);
        ImGui::DockBuilderDockWindow("Simulation window", dock_id_simulation);
        ImGui::DockBuilderDockWindow("Logger", dock_id_logger);

        ImGui::DockBuilderFinish(dock_id_simulation);

    }

    ImVec2 sim_window_pos = ImGui::GetCursorScreenPos();

    {
        ImGui::Begin("Simulation window");

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        sim_window_pos = ImGui::GetCursorScreenPos();
        ImVec2 window_size = ImGui::GetWindowSize();

        logicTimeStart = std::chrono::high_resolution_clock::now(); 
        simulation->update(isSimulationRunning);
        simulation->afterUpdate();
        logicTimeEnd = std::chrono::high_resolution_clock::now();
        logicTimeElapsed = logicTimeEnd - logicTimeStart;
        logicFramerate = 1000.0 / (logicTimeElapsed.count() == 0.0 ? 0.00001 : logicTimeElapsed.count());

        // Move camera whe arrow keys pressed and windwo focused
        if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
            int cameraDirX = 0;
            int cameraDirY = 0;
            if (ImGui::IsKeyDown(ImGuiKey_LeftArrow)) {
                cameraDirX -= 1;
            }
            if (ImGui::IsKeyDown(ImGuiKey_RightArrow)) {
                cameraDirX += 1;
            }
            if (ImGui::IsKeyDown(ImGuiKey_UpArrow)) {
                cameraDirY -= 1;
            }
            if (ImGui::IsKeyDown(ImGuiKey_DownArrow)) {
                cameraDirY += 1;
            }
            if (cameraDirX || cameraDirY) {
                simulation->camera.move(cameraDirX, cameraDirY);
            }
            handleBotKeysEvent(simulation);
        }

        simulation->render(draw_list, sim_window_pos, window_size);


        ImGui::End();
    }
    
    {
        // Information window
        ImGui::Begin("Information");
        if (ImGui::BeginTabBar("Information")) {
            if (ImGui::BeginTabItem("Main")) {
                // Simulation properties
                if (ImGui::CollapsingHeader("Simulation properties")) {
                    ImGui::Text("MapSize (%.1f, %.1f)", simulation->chunkManager->mapWidth, simulation->chunkManager->mapHeight);
                    ImGui::Text("UnitSize: %i", simulation->unit);
                    ImGui::Separator();
                    ImGui::Text("NumberOfChunksX: %i", simulation->chunkManager->numberOfChunksX);
                    ImGui::Text("NumberOfChunksY: %i", simulation->chunkManager->numberOfChunksY);
                    ImGui::Text("ChunkSize: %.1f | (10 * UnitSize)", simulation->chunkManager->chunkSize);

                    ImGui::Dummy(ImVec2(0.0f, 10.0f));
                    ImVec2 mouse_pos = ImGui::GetMousePos();
                    ImGui::Text("RelativeMousePos: (%.1f, %.1f)", mouse_pos.x - sim_window_pos.x, mouse_pos.y - sim_window_pos.y);

                    ImGui::Dummy(ImVec2(0.0f, 10.0f));
                    simulation->camera.drawCameraControls();

                    ImGui::Dummy(ImVec2(0.0f, 20.0f));
                }
                // FPS tab
                ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                if (ImGui::CollapsingHeader("Efficiency")) {
                    
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4((1.0f - std::min(1.0f, io.Framerate / 120.0f)) * 0.9, std::min(1.0f, io.Framerate / 120.0f) * 0.9, 0.0f, 1.0f));
                    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
                    ImGui::PopStyleColor();
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4((1.0f - std::min(1.0, logicFramerate / 120.0f)) * 0.9, std::min(1.0, logicFramerate / 120.0f) * 0.9, 0.0f, 1.0f));
                    ImGui::Text("Simulation logic average %.4f ms/frame (%.1f FPS)", logicTimeElapsed.count(), logicFramerate);
                    ImGui::PopStyleColor();
                    ImGui::Dummy(ImVec2(0.0f, 20.0f));
                    ImGui::Text("Number of objects: %i", simulation->getNumberOfObjects());
                    ImGui::Dummy(ImVec2(0.0f, 20.0f));
                }

                if (ImGui::CollapsingHeader("Objects List")) {
                    createObjectListGui(simulation);
                }
                
                // Object info tab
                ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                if (ImGui::CollapsingHeader("Object Info")) {
                    ImGui::Dummy(ImVec2(0.0f, 20.0f));
                    auto objectToDisplayInfo = simulation->getSelectedObject();
                    auto selectedChunk = simulation->getSelectedChunk();
                    if (objectToDisplayInfo) {
                        objectToDisplayInfo->displayInfo();
                    }
                    else if (selectedChunk) {
                        selectedChunk->displayInfo();
                    }
                }
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        
        ImGui::End();
    }

    {
        ImGui::Begin("Logger");
        simulation->drawLogger("Logger");
        ImGui::End();
    }
}

void createObjectListGui(std::shared_ptr<Simulation> simulation) {
    bool objectSelected = false;
    unsigned long selectedID = 0;
    if (auto selectedObject = simulation->getSelectedObject()) {
        selectedID = simulation->getSelectedObject()->id.get();
        objectSelected = true;
    }
     
    {
        ImGui::BeginChild("left pane", ImVec2(150, 200), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX | ImGuiChildFlags_ResizeY);
        // for (int i = 0; i < 100; i++)
        // {
        //     // FIXME: Good candidate to use ImGuiSelectableFlags_SelectOnNav
        //     char label[128];
        //     sprintf(label, "MyObject %d", i);
        //     if (ImGui::Selectable(label, selected == i))
        //         selected = i;
        // }
        auto simulationObjects = simulation->getObjects();
        // simulation->log(Logger::LOG, "Object list size: %i", simulationObjects.get()->size());
        for (auto obj : *simulationObjects.get()) {
            char label[128];
            sprintf(label, "[%0*lu] %s", 6, obj->id.get(), getTypeString(obj->type()));
            if (ImGui::Selectable(label, objectSelected && selectedID == obj->id.get())) {
                selectedID = obj->id.get();
                simulation->selectSingleObject(obj);
            }
        }
        
        ImGui::EndChild();
    }
    ImGui::SameLine();
    // Right
    {
        ImGui::BeginGroup();
        ImGui::BeginChild("item view", ImVec2(0, 200)); // Leave room for 1 line below us
        ImGui::Text("Object: %0*lu", 6, selectedID);
        ImGui::Separator();
        if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
        {
            if (ImGui::BeginTabItem("Description"))
            {
                ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ");
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Details"))
            {
                ImGui::Text("ID: 0123456789");
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::EndChild();
        if (ImGui::Button("Delete")) {
            if (auto selectedObject = simulation->getSelectedObject()) {
                selectedObject->markForDeletion();
                // simulation->log(Logger::LOG, "Object [%0*lu] marked for deletion\n", 6, selectedObject->id.get());
            }
        }
        ImGui::SameLine();
        // if (ImGui::Button("Save")) {}
        ImGui::EndGroup();
    }
}

void handleBotKeysEvent(std::shared_ptr<Simulation> simulation) {
    // Move current bot
    auto selectedObject = simulation->getSelectedObject();
    if (selectedObject && selectedObject->type() == SimulationObjectType::BotObject) {
        auto selectedBot = std::dynamic_pointer_cast<BotObject>(selectedObject);
        int moveDirX = 0;
        int moveDirY = 0;
        if (ImGui::IsKeyDown(ImGuiKey_A)) {
            moveDirX -= 1;
        }
        if (ImGui::IsKeyDown(ImGuiKey_D)) {
            moveDirX += 1;
        }
        if (ImGui::IsKeyDown(ImGuiKey_W)) {
            moveDirY -= 1;
        }
        if (ImGui::IsKeyDown(ImGuiKey_S)) {
            moveDirY += 1;
        }
        if (moveDirX || moveDirY) {
            selectedBot->actionMove(Vec2<float>(moveDirX, moveDirY), 1.0f);
        }
        if (ImGui::IsKeyDown(ImGuiKey_Q)) {
            selectedBot->actionAttack();
        }
        if (ImGui::IsKeyDown(ImGuiKey_E)) {
            selectedBot->actionEat();
        }
        if (ImGui::IsKeyDown(ImGuiKey_1)) {
            // selectedBot->actionSpawnBot();
            simulation->log(Logger::ERROR, "Spawn of bots with keys didnt setuped!");
        }
        if (ImGui::IsKeyDown(ImGuiKey_X)) {
            selectedBot->actionSuicide();
        }
    }
}