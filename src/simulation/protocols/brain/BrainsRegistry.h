#pragma once

#include "BotBrain.h"

#include <functional>
#include <map>

// Registry for bot creation
class BrainsRegistry {
public:
    using BrainFactory = std::function<std::shared_ptr<BotBrain>()>;

    static BrainsRegistry& getInstance() {
        static BrainsRegistry instance;
        return instance;
    }

    void registerBot(const std::string& name, BrainFactory factory) {
        botFactories[name] = factory;
    }

    std::shared_ptr<BotBrain> createBot(const std::string& name) const {
        auto it = botFactories.find(name);
        if (it != botFactories.end()) {
            return it->second();
        }
        throw std::runtime_error("Bot class not found: " + name);
    }

    std::vector<std::string> listRegisteredBots() const {
        std::vector<std::string> names;
        for (const auto& pair : botFactories) {
            names.push_back(pair.first);
        }
        return names;
    }

private:
    std::map<std::string, BrainFactory> botFactories;

    // Singleton, so prevent direct construction
    BrainsRegistry() = default;
    BrainsRegistry(const BrainsRegistry&) = delete;
    BrainsRegistry& operator=(const BrainsRegistry&) = delete;
};

#define REGISTER_BOT_CLASS(CLASS) \
    namespace { \
        struct CLASS##Registrar { \
            CLASS##Registrar() { \
                auto botInstance = std::make_shared<CLASS>(); \
                BrainsRegistry::getInstance().registerBot(botInstance->populationName, []() { \
                    return std::make_shared<CLASS>(); \
                }); \
            } \
        }; \
        static CLASS##Registrar global_##CLASS##Registrar; \
    }
