#include "Bot.h"
#include "objects/SimulationObject.h"
#include "objects/Food.h"

void BotObject::onDestroy() {
        if (auto validSimulation = simulation.lock()) {
            float calories = health.getMax() * 0.3 + food.get() * 0.7;
            // validSimulation->addObject(SimulationObjectType::FoodObject, validSimulation, pos, colorInt(100, 0, 0), calories, calories, 0, 5.0f, true);
            validSimulation->addObject(SimulationObjectType::FoodObject,
                            std::make_shared<FoodObject>(
                                validSimulation,
                                pos,
                                colorInt(100, 0, 0),
                                calories,
                                calories,
                                0,
                                5.0f,
                                true));
        }
    }