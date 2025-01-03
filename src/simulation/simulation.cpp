#include "simulation.h"

// Here will be defined functions that cant be defined in header

void SimulationObject::setAsInfoViewObject() {
    if (auto sim = simulation.lock()) {
        sim->setViewInfoObject(shared_from_this());
    }
}