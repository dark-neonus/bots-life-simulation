#pragma once

enum class SpawnType
{
	Random, 
	Circle,
	OnePlace
};

struct MapGenerationSettings
{
	SpawnType spawnType = SpawnType::Random;
	unsigned int numberOfBotsPerPopulation = 5;
	float spawnRadius = 100.0f;
	
	// float numberOfTreePerChunk = 3;

	bool randomSpawnFood = false;
	float foodPerChunk = 3;
	float foodSpawnChance = 0.3f;

	/// @brief Threshold after which tree will be spawned
	float perlinThreshold = 0.5f;
	/// @brief Scale of perlin noise positive value
	float positiveScale = 0.1f;
	/// @brief Scale of perlin noise negative value 
	float negativeScale = 0.35f;
	/// @brief Indicate how rare will be trees
	/// @example if (RandomValueInRange(0, treeRarety) == 0) { spawTree(); }
	unsigned int treeRarety = 5;

};