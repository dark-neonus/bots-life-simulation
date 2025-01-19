#pragma once

class Dota2Player : public BotBrain {
private:
    inline static int highestTreeCount = 0;
    inline static bool isHungry = false;

    static int population;
    static int death;
    static unsigned long b_id;

    inline static Vec2<float> homeBase;
    inline static float safeDistance = 100.0f;

    void printStats() {
        std::cout << "<" << populationName;
        std::cout << "> |- population: " << population;
        std::cout << " | death: " << death;
        std::cout << " | b_id: " << b_id << " |\n";
    }

    // Coordinate with allies in the vicinity and move towards home if needed
    bool coordinateWithAllies(UpdateProtocol& data, UpdateProtocolResponce& responce) {
        int allyCount = 0;

        for (auto& bot : data.visibleBots) {
            if (isSamePopulation(bot)) {
                allyCount++;

                if (bot->isUnderAttack()) {
                    responce.actionGoTo(homeBase);
                    return true;
                }
            }
        }

        return false;
    }

    bool avoidEnemies(UpdateProtocol& data, UpdateProtocolResponce& responce) const {
        for (const auto& enemy : data.visibleEnemies) {
            // Move away from enemies if they are too close
            if (data.body->pos().distanceTo(enemy->pos()) < safeDistance) {
                Vec2<float> awayFromEnemy = data.body->pos() - enemy->pos();
                awayFromEnemy.normalize();
                responce.actionGoTo(awayFromEnemy);
                return true;
            }
        }
        return false;
    }

    bool gatherFood(UpdateProtocol& data, UpdateProtocolResponce& responce) {
        if (data.visibleFood.empty() || !isHungry) {
            return false;
        }
           
        if (canReach(data.nearestFood)) {
            responce.actionEatByID(data.nearestFood->id());
        }
        else {
            responce.actionGoTo(data.nearestFood->pos());
        }

        return true;
    }

    void adaptivePatrol(UpdateProtocol& data, UpdateProtocolResponce& responce) {
        static float angle = 0.0f;
        static const float radius = 500.0f;

        angle += 0.02f;
        if (angle >= 2 * 3.14159f) {
            angle -= 2 * 3.14159f;
        }

        float patrolX = homeBase.x + radius * cos(angle);
        float patrolY = homeBase.y + radius * sin(angle);
        Vec2<float> patrolDirection(patrolX, patrolY);

        responce.actionGoTo(patrolDirection);
    }


    void updateHomeBaseLocation(UpdateProtocol& data) {
        Vec2<float> bestHomeBase(0.0f, 0.0f);
        int treeCount = 0;

        Vec2<float> totalFoodPos(0.0f, 0.0f);

        for (const auto& tree : data.visibleTree) {
            totalFoodPos += tree->pos();
            treeCount++;
        }

        if (treeCount > highestTreeCount) {
            bestHomeBase = totalFoodPos / treeCount;
            highestTreeCount = treeCount;
            homeBase = bestHomeBase;
        }
    }

    // Attack logic based on allies and enemies
    bool attackIfNeeded(UpdateProtocol& data, UpdateProtocolResponce& responce) {
        int allyCount = 1;
        int enemyCount = 0;
        float totalEnemyHealth = 0.0f;
        float totalAllyHealth = data.body->health();

        std::shared_ptr<const ShadowBotObject> weakestEnemy;
        float lowestHealth = std::numeric_limits<float>::max();

        // Count allies and enemies and their health
        for (const auto& bot : data.visibleBots) {
            if (isSamePopulation(bot)) {
                allyCount++;
                totalAllyHealth += bot->health();
            }
            else {
                enemyCount++;
                totalEnemyHealth += bot->health();
                float health = bot->health();
                if (health < lowestHealth) {
                    lowestHealth = health;
                    weakestEnemy = bot;
                }
            }
        }

        // Attack logic
        if (enemyCount > 0 && allyCount > enemyCount) {
            // If allies outnumber enemies, attack the nearest enemy
            if (data.nearestEnemy) {
                if (!canReach(data.nearestEnemy)) {
                    responce.actionGoTo(data.nearestEnemy->pos());
                }
                else {
                    responce.actionAttackNearest(false);
                }
                return true;
            }
        }
        else if (enemyCount > 0 && totalEnemyHealth < totalAllyHealth) {
            // If the enemy is weaker, attack the nearest enemy
            if (weakestEnemy) {
                if (!canReach(weakestEnemy)) {
                    responce.actionGoTo(weakestEnemy->pos());
                }
                else {
                    responce.actionAttackByID(false, weakestEnemy->id());
                }

                return true;
            }
        }

        return false;
    }

    bool spawnNewBot(UpdateProtocol& data, UpdateProtocolResponce& responce) {
        float distanceToHomeBase = data.body->pos().distanceTo(homeBase);
        if (data.body->food() >= data.body->maxFood() * 0.9 && distanceToHomeBase < safeDistance) {
            responce.actionSpawn(
                std::dynamic_pointer_cast<BotBrain>(
                    std::make_shared<Dota2Player>()
                ),
                protocolsHolder->initProtocol.evolutionPoints + 5
            );

            return true;
        }

        return false;
    }

public:
    Dota2Player()
        : BotBrain("CleverStrategist") {}

    void init(InitProtocol& data, InitProtocolResponce& responce) override {
        responce.r = 204;
        responce.g = 0;
        responce.b = 0;

        responce.healthPoints = int(0.1 * data.evolutionPoints);
        responce.foodPoints = int(0.3 * data.evolutionPoints);
        responce.visionPoints = int(0.4 * data.evolutionPoints);
        responce.speedPoints = int(0.15 * data.evolutionPoints);
        responce.attackPoints = int(0.05 * data.evolutionPoints);

        population++;
        b_id++;
        printStats();
    }

    void update(UpdateProtocol& data, UpdateProtocolResponce& responce) override {

        if (data.body->food() < data.body->maxFood() * 0.5) {
            isHungry = true;
        } else if (data.body->food() > data.body->maxFood() * 0.9) {
            isHungry = false;
        }

        if (gatherFood(data, responce)) {
            return;
        }

        if (attackIfNeeded(data, responce)) {
            return;
        }

        if (avoidEnemies(data, responce)) {
            return;
        }

        updateHomeBaseLocation(data);

        if (spawnNewBot(data, responce)) {
            return;
        }

        if (coordinateWithAllies(data, responce)) {
            return;
        }

        adaptivePatrol(data, responce);
    }

    void kill(KillProtocol& data, KillProtocolResponce& responce) override {
        population--;
        death++;
        printStats();
        responce.success = true;
    }
};

// Init values of static brain members
int Dota2Player::population = 0;
int Dota2Player::death = 0;
unsigned long Dota2Player::b_id = 0;
//Vec2<float> Dota2Player::homeBase(50.0f, 50.0f);  // Initial home base location
