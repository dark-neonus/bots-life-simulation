# Intro
This simulation allow you to create brain that will control your bots. For now game is without any winning mechanics, so you can count situation when you destroy all other populations and survived for some time as winning situation.

# Structure
## Simulation Backend
As user, you dont need to know a lot about backend code. Main things that you may need there are `simulation/settings` directory in which lays settings of simulation. If you need to change any simulation settings, go there to search for requried parameters and change them in `main.cpp` where simulation runs.

## Where to write code
Your main purpose is to write brains for bots. We provide `brains` directory for your brains code. There are some examples of brains in `brains/exampes` directory and we recommend to start your brain code in copy of `brains/examples/Base.h` file. There you can find more detailed instructions on how to write bots brain.

## How to put everything together
When you done writting your bots brain, you need to register it. It can be done next way:

In `BotRegister.h` include header with your bots brain to get access to brain class
```h
// Include your brains here
#include "brains/YourBotBrain.h"
```
and use `REGISTER_BOT_CLASS()` macros to add bots with your brain to smulation:
```h
// Register your brains here
REGISTER_BOT_CLASS(YourBotBrainClass);
```
# Concepts

## Simulation
In simple words simulation is bunch of objects that are being updated and drawn.
There is parent class for all simulation objects called `SimulationObject`.
All other simulation object classes inherit from it.
Here are all simulation object classes:
- `FoodObject` - object that contain calories. Can grow and decay(when calories reach 0, object is deleted from simulation). Can be eaten by bots and is being spawned by trees or in random places depending on simulation settings. Food objects on map appear as squares of usually green or red colors.
- `TreeObject` - eternal(undestroyable) object, that periodically spawn food objects, in amount depending on `numberOfFruits` parameter(on map represented by number of shapes vertices).
- `BotObject` - object, that represent bot. Have brain created by users. Can perform different actions, such as move, eat, attack, spawn and suicide. Have health, food, vision, speed and attack stats depending on evolution points given to bot when it was born.
## BotObject detailed

### Stats
1. `health` - indicate how many health points have bot. When this parameter drops to 0, bot object die and simulation delete it.
2. `food` - indicate amount of calories that bot can store(stomach capacity). If it drops to 0, bot start to lose hp very fast.
3. `seeDistance/vision` - indicate radius in which objects are visible for bot.
4. `speed` - indicate maximal movement step per frame for bot.
5. `attack` - indicate amount of points, that will be substracted from health of bot, that is attacked by current bot.

### Actions
All interactions between bot and simulation, are controlled by brain.
(even such mechanics as eating).
Each action(as well, as doing nothing), use specific amount of calories.
Bot can perform following actions:
1. `DoNothing` - tell bot to do nothing this frame.
2. `Move` - tell bot to move in given direction with given speed multiplier.
3. `GoTo` - tell bot to move to given position.
4. `EatNearest` - tell bot to eat nearest food object.
If there is no food objects in interact radius, bot will do nothing.
5. `EatByID` - tell bot to eat food object with given ID.
If there is no food object with given ID or there is no food object in interact radius,
bot will do nothing.
6. `AttackNearest` - tell bot to attack nearest bot.
If there is no bots in interact radius, bot will do nothing.
7. `AttackByID` - tell bot to attack bot with given ID. 
If there is no bot with given ID or there is no bot in interact radius, bot will do nothing.
8. `Spawn` - tell bot to spawn new bot with given brain and evolution points.
If there is not enough food to give such amount of evoulation points, health will be used too.
If there is not enough health to give such amount of evoulation points, bot will die and
new bot wont be born.
9. `Suicide` - tell bot to kill itself at the end of frame.

### ShadowObjects
Each simulation object have its own shadow object(e.g. BotObject have ShadowBotObject).
Shadow object are used to represent objects for bots. It prevenent bots from accessing and
changing real simulation objects.

### Bots vision
In order to be able to inteact with some objects bot need to see it.
So, each frame, for each bot, simulation calculate sets of objects in vision of bot.
Next sets available for use in ots brain:
1. `visibleObjects` - set of all simulation objects in seeDistance radius of bot.
2. `visibleFood` - set of all food objects in seeDistance radius of bot.
3. `visibleTree` - set of all tree objects in seeDistance radius of bot.
4. `visibleBots` - set of all bot objects in seeDistance radius of bot.
5. `visibleFriends` - set of all bot objects from same population in seeDistance radius of bot.
6. `visibleEnemies` - set of all bot objects from different populations in seeDistance radius of bot.

Also to simplify users life, simulation provide precalculated objects of each type
that are nearest to bot and distance to them. If there is no object of given type in
seeDistance radius, nearest object is nullptr and distance to it is -1.0f.
Available nearest objects:
1. `nearestFood`, `distanceToNearestFood` - nearest food object and distance to it.
2. `nearestTree`, `distanceToNearestTree` - nearest tree object and distance to it.
3. `nearestBot`, `distanceToNearestBot` - nearest bot object and distance to it.
4. `nearestFriend`, `distanceToNearestFriend` - nearest friend bot object and distance to it.
5. `nearestEnemy`, `distanceToNearestEnemy` - nearest enemy bot object and distance to it.

## General recomendations for brains

1. Distribure evolution points in init() wisely.
Dont spend too much points on one stat and too little on another(except of cases
where you have specific tactic). Make sure there are enough point for `food` and `vision`
stats for bot to survive.

2. Staying in one place in most cases is bad idea. If bot see food, go to it and eat.
If bot see bot, attack it and eat remainings. If bot see tree, go to it and search/wait fot food.
If there is nothing in sign, go somewhere and search for food, because after some time bot
will start to starve.

3. If possible, try to limit number of bots. If there are too many bots, they will eat all food
and some of them will start to starve. In worse conditions(rare food spawn), it can cause
population exctinsion.

4. Same with enemies. If there are too many enemies, they will eat all food and your bots will
start to starve.

5. Try to spawn new bots, when everything good with food. More bots, will make
population exctinsion less likely.

6. When spawning new bots, give them more evolution points, than you have. It will cause
evolution process that will make your population stronger.
(For this tactic to work, you must distribute evolution points not as constant amount,
but as part of given points, like `0.2f * amountOfPoints`).

7. You can implement more complex bots behaviors like `hive mind`, `groups` and `minions spawn`.

8. You can implement more complex bots brains like hive mind, groups and minions spawn.
    - `Hive mind` - can be implemented using static members in bots brain class or
                    using additional class that will be shared between all brains of population.
    - `Groups` - can be implemented by interation between bots from same population in vision.
    - `Minions spawn` - can be implemented by spawning bots with different brains,
                        that will do simple role like attacking or accumulation calories.
                        To protect minions, bots will need to check for enemy type, before attacking.
    
    All this ideas are hard to implement and make practical, but you can try your skills.

