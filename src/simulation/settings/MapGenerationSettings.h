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
	
	float numberOfTreePerChunk = 3;

	bool randomSpawnFood = false;
	float foodPerChunk = 3;
	float foodSpawnChance = 0.3f;
};