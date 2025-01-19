#include "protocols/brain/BrainsRegistry.h"

// Include your brains here
#include "brains/examples/AggressiveMultiplier.h"
#include "brains/examples/AggressiveMultiplier2.h"
#include "brains/examples/Dota2Player.h"

// Register your brains here
REGISTER_BOT_CLASS(AggressiveMultiplierBotBrain);
REGISTER_BOT_CLASS(AggressiveMultiplierBotBrain2);