#include "simulation.h"

// here we define function that cant be define in simulation.h
const char* getTypeString(SimulationObjectType type) {
    return SimulationObjectTypeNames[static_cast<int>(type)];
}

void SimulationObject::drawHighlightion(ImDrawList *draw_list, ImVec2 drawing_delta_pos) {
    // static constexpr int selectionRubHeigt = 3;
    // static constexpr int selectionRubWidth = 2;
    Vec2<float> realPos = Vec2<float>(drawing_delta_pos) + pos;
    if (auto validSimulation = simulation.lock()) {
        int selectionRadius = getRadius() + validSimulation->allowedClickError;
        draw_list->AddRect(toImVec2(realPos - selectionRadius), toImVec2(realPos + selectionRadius), ImColor(selectionColor), 0, 0, 3.0f);
        draw_list->AddCircleFilled(toImVec2(realPos), getRadius(), ImColor(colorInt(0, 255, 255, 50)), 18);
        draw_list->AddRect(toImVec2(realPos - 1), toImVec2(realPos + 1), ImColor(colorInt(255, 0, 255, 100)));
    }
}

void SimulationObject::markForDeletion() {
    if (auto validSimulation = simulation.lock()) {
        validSimulation->addToDeathNote(shared_from_this());
    }
}

// Here we define just big functions ot those, which implementation in simulation.h isnt representative

void Simulation::render(ImDrawList *draw_list, ImVec2 window_pos, ImVec2 window_size, bool drawDebugLayer) {

    camera.setSize(window_size.x - 20, window_size.y - 40);
    camera.update();
    ImVec2 drawing_delta_pos = ImVec2(window_pos.x - camera.x(), window_pos.y - camera.y());

    ImVec2 mouse_pos = ImGui::GetMousePos();
    ImVec2 mouse_map_pos = ImVec2(mouse_pos.x + camera.x(), mouse_pos.y + camera.y());
    // Holder variable for distance between click position and object
    float dist_sq = 0.0f;
    ImVec2 object_center;
    // In future, when we will have camera will be able to move it
        
    bool isMouseClicked = ImGui::IsMouseClicked(0);
    // Check if click was inside simulation window. If not, then not count it as a click
    if (mouse_pos.x < window_pos.x || mouse_pos.y < 0 ||
        mouse_pos.x >= window_pos.x + window_size.x - 10 || mouse_pos.y >= window_pos.y + window_size.y - 10)
        { isMouseClicked = false; }

    
    // Click handling
    bool wasSelectedObject = false;
    if (isMouseClicked) {
        // If mouse were clicked clear all previous selections
        selectedObjects.clear();
        selectedChunk.reset();
        std::shared_ptr<Chunk> clickedChunk = chunkManager.whatChunkHere(Vec2<float>(mouse_map_pos) - Vec2<float>(window_pos));
        if (clickedChunk) {
            for (auto& obj : clickedChunk->objects) {
                if (auto validObj = obj.lock()) {
                    object_center = ImVec2(window_pos.x + validObj->pos.x, window_pos.y + validObj->pos.y);
                    dist_sq = (mouse_map_pos.x - object_center.x) * (mouse_map_pos.x - object_center.x) + 
                                (mouse_map_pos.y - object_center.y) * (mouse_map_pos.y - object_center.y);
                    if (dist_sq <= (validObj->getRadius() + allowedClickError) * (validObj->getRadius() + allowedClickError)) {
                        // setViewInfoObject(validObj);
                        selectedObjects.push_back(validObj);
                        wasSelectedObject = true;
                    }
                }
            }
            // if chunk was clicked, but no specific object was selected
            if (!wasSelectedObject) {
                selectedChunk = clickedChunk;
                for (auto& obj : clickedChunk->objects) {
                    if (auto validObj = obj.lock()) {
                        selectedObjects.push_back(validObj);
                    }
                }
            }
        }
    }
    // Draw map mesh
    chunkManager.drawChunksMesh(draw_list, drawing_delta_pos);

    // If chunk is selected, draw it before anything else
    if (auto validSelectedChunk = selectedChunk.lock()) {
        draw_list->AddRect(toImVec2(Vec2<float>(drawing_delta_pos) + validSelectedChunk->startPos),
                                toImVec2(Vec2<float>(drawing_delta_pos) + validSelectedChunk->endPos), ImColor(colorInt(255, 255, 0, 50)), 0, 0, 2);
    }
    
    // Draw all objects by chunks
    for (auto chunk : chunkManager) {
        for (auto& obj : chunk->objects) {
            if (auto validObj = obj.lock()) {
                validObj->draw(draw_list, drawing_delta_pos);
                
            }
        }
    }
    
    // Draw debug layer
    if (drawDebugLayer) {
        for (auto& obj : selectedObjects)
        {
            if (auto validSelectedObject = obj.lock()) {
                validSelectedObject->drawHighlightion(draw_list, drawing_delta_pos);
            }
        }
    }
}
