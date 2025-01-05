#include "simulation.h"

// here we define function that cant be define in simulation.h

void SimulationObject::drawHighlightion(ImDrawList *draw_list, ImVec2 window_pos) {
        // static constexpr int selectionRubHeigt = 3;
        // static constexpr int selectionRubWidth = 2;
        Vec2<float> realPos = Vec2<float>(window_pos) + pos;
        if (auto validSimulation = simulation.lock()) {
            int selectionRadius = getRadius() + validSimulation->allowedClickError;
            draw_list->AddRect(toImVec2(realPos - selectionRadius), toImVec2(realPos + selectionRadius), ImColor(selectionColor), 0, 0, 3.0f);
                                
        }
    }