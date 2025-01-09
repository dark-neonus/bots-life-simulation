#include "chunks.h"

void Chunk::removeObject(std::shared_ptr<SimulationObject> obj) {
        auto it = std::find_if(objects.begin(), objects.end(),
            [&obj](const std::weak_ptr<SimulationObject>& wp) {
                auto locked = wp.lock();
                return locked == obj;     
            });

        if (it != objects.end()) {
            objects.erase(it);
        }
    }