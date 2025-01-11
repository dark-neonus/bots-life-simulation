// #pragma once

// #include <random>
// #include <cmath>

// #include "protocols/brain/BotBrain.h"

// int population = 0;

// unsigned long b_id = 0;

// class MultiplierBotBrain : public BotBrain
// {
// public:
//     std::random_device rd;
//     std::mt19937 gen;
//     std::uniform_real_distribution<float> deltaAngle = std::uniform_real_distribution<float>(-0.02f, 0.02f);
//     std::uniform_real_distribution<float> circleAngle = std::uniform_real_distribution<float>(-M_PI, M_PI);
//     std::uniform_int_distribution<int> randomDirChance = std::uniform_int_distribution<int>(0, 200);
//     float angle = 0.0f;

//     /// @brief User defined brain class must have constructor that takes 0 arguments and use ": BotBrain()"
//     MultiplierBotBrain() 
//     : BotBrain(), 
//       gen(std::mt19937(static_cast<std::mt19937::result_type>(rd() + ++b_id))) 
//     {
//     }

//     void init() override 
//     { // User defined brain class must have override init function that takes 0 arguments
//         protocolsHolder->initProtocolResponce.r = 50;
//         protocolsHolder->initProtocolResponce.g = 200;
//         protocolsHolder->initProtocolResponce.b = 130;

//         protocolsHolder->initProtocolResponce.healthPoints = 5;
//         protocolsHolder->initProtocolResponce.foodPoints = 39;
//         protocolsHolder->initProtocolResponce.visionPoints = 35;
//         protocolsHolder->initProtocolResponce.speedPoints = 20;
//         protocolsHolder->initProtocolResponce.attackPoints = 1;

//         angle = std::fmod(deltaAngle(gen) * 3 + M_PI, 2 * M_PI) - M_PI;
//         population++;
//         std::cout << "population: " << population << "\n";
//     }

//     int focusTime = 0;

//     void update() override
//     { // User defined brain class must have override update function that takes 0 arguments
//         if (randomDirChance(gen) == 7) {
//             focusTime = 200;
//             angle = circleAngle(gen);
//         }
//         if (protocolsHolder->updateProtocol.body->food() <= 0.5f * protocolsHolder->updateProtocol.body->maxFood()) {
//             focusTime = 0;
//         }
//         if (focusTime > 0) {
//             focusTime--;
//             protocolsHolder->updateProtocolResponce.actionMove(Vec2<float>(std::cos(angle), std::sin(angle)), 1.0f);
//             return;
//         }
        
//         if (protocolsHolder->updateProtocol.distanceToNearestFood == -1.0f) {
//             angle += deltaAngle(gen);
//             angle = std::fmod(angle + M_PI, 2 * M_PI) - M_PI;
//             protocolsHolder->updateProtocolResponce.actionMove(Vec2<float>(std::cos(angle), std::sin(angle)), 1.0f);
//         }
//         else {
//             if (protocolsHolder->updateProtocol.distanceToNearestFood == 0.0f) {
//                 if (protocolsHolder->updateProtocol.body->food() >= 0.9f * protocolsHolder->updateProtocol.body->maxFood() &&
//                     protocolsHolder->updateProtocol.body->health() == protocolsHolder->updateProtocol.body->maxHealth()) {
//                     protocolsHolder->updateProtocolResponce.actionSpawn(
//                         std::dynamic_pointer_cast<BotBrain>(
//                             std::make_shared<MultiplierBotBrain>()
//                         )
//                     );
//                 }
//                 else {
//                     protocolsHolder->updateProtocolResponce.actionEatNearest();
//                 }
//             }
//             else {
//                 // if (protocolsHolder->updateProtocol.body->food)
//                 // if (protocolsHolder->updateProtocol.distanceToNearestBot == 0.0f) {
//                 //     protocolsHolder->updateProtocolResponce.actionGoTo(
//                 //         protocolsHolder->updateProtocol.nearestFood->pos() + Vec2<float>(std::cos(angle), std::sin(angle)) * 10);
//                 // }
//                 // else {
//                 //     protocolsHolder->updateProtocolResponce.actionGoTo(protocolsHolder->updateProtocol.nearestFood->pos());
//                 // }
//                 protocolsHolder->updateProtocolResponce.actionGoTo(protocolsHolder->updateProtocol.nearestFood->pos());
//             }
//         }
//     }

//     void kill() override
//     { // User defined brain class must have override kill function that takes 0 arguments
//         population--;
//         std::cout << "population: " << population << "\n";
//     }
// };
