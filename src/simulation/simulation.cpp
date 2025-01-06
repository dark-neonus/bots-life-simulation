#include "simulation.h"

// here we define function that cant be define in simulation.h

void SimulationObject::drawHighlightion(ImDrawList *draw_list, ImVec2 window_pos) {
        // static constexpr int selectionRubHeigt = 3;
        // static constexpr int selectionRubWidth = 2;
        Vec2<float> realPos = Vec2<float>(window_pos) + pos;
        if (auto validSimulation = simulation.lock()) {
            int selectionRadius = getRadius() + validSimulation->allowedClickError;
            draw_list->AddRect(toImVec2(realPos - selectionRadius), toImVec2(realPos + selectionRadius), ImColor(selectionColor), 0, 0, 3.0f);
            draw_list->AddCircleFilled(toImVec2(realPos), getRadius(), ImColor(colorInt(0, 255, 255, 50)), 18);
            draw_list->AddRect(toImVec2(realPos - 1), toImVec2(realPos + 1), ImColor(colorInt(255, 0, 255, 100)));
        }
    }