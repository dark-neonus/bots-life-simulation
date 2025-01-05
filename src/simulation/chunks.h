#pragma once

#include "utilities/utilities.h"
#include "simulation.h"

#include <memory>
#include <vector>

class SimulationObject;
class Simulation;

class Chunk
{
private:
public:
    const Vec2<float> startPos;
    const Vec2<float> endPos;
    const float chunkSize;

    std::vector<std::weak_ptr<SimulationObject>> objects;

    Chunk(float startX, float startY, float chunkSize_)
        : startPos(startX, startY), chunkSize(chunkSize_), endPos(startX + chunkSize, startY + chunkSize)
    {
    }
    /// @brief Add given object to objects vector and assigne self to objects chunk property
    /// @param obj Object to add
    void addObject(std::weak_ptr<SimulationObject> obj) {}

    /// @brief Move object from current chunk to another
    /// @param objectToMove Object of current chunk to move
    /// @param destinationChunk Chunk for moving given object to
    void moveToChunk(std::weak_ptr<SimulationObject> objectToMove, std::weak_ptr<Chunk> destinationChunk) {}

    /// @brief Check if given object in this chunk. Use to check if object gone to another chunk after movement
    /// @param obj Object to check
    /// @return true if object in this cunk, false otherwise
    bool isObjectInChunk(std::weak_ptr<SimulationObject> obj) { return false; } // Bad value. Just to compile
};

class ChunkManager
{
private:

    std::vector<std::vector<Chunk>> chunks;
public:
    // std::weak_ptr<Simulation> simulation;

    const float chunkSize;
    const int numberOfChunksX;
    const int numberOfChunksY;

    const float mapWidth;
    const float mapHeight;

    ChunkManager(int numberOfChunksX_, int numberOfChunksY_, float chunkSize_)
        : numberOfChunksX(numberOfChunksX_), numberOfChunksY(numberOfChunksY_), chunkSize(chunkSize_),
        mapWidth(numberOfChunksX * chunkSize), mapHeight(numberOfChunksY * chunkSize)
    {
        // Init chunks
        chunks.resize(numberOfChunksY);
        for (int y = 0; y < numberOfChunksY; y++)
        {
            for (int x = 0; x < numberOfChunksX; x++)
            {
                chunks[y].emplace_back(x * chunkSize, y * chunkSize, chunkSize);
            }
        }
    }

    /// @brief Return pointer to chunk from chunk matrix by coordinates (x, y)
    /// @param xIndex Column index of chunk
    /// @param yIndex Row index of chunk
    /// @return Return pointer to chunk at chunks[yIndex][xIndex]. If indexes are invalid, return std::weak_ptr<Chunk>() meaning nullptr
    std::weak_ptr<Chunk> getChunk(int xIndex, int yIndex) {
        // When directly access chunk from chunks vector in function implementation, need to switch x and y places, e.g [y][x]
        return std::weak_ptr<Chunk>();  // Bad value. Just to compile
    }

    /// @brief Get and return chunk which own given position
    /// @param position Position to check
    /// @return Return chunk which contain given position. If there is no such chunk, return std::weak_ptr<Chunk>() meaning nullptr
    std::weak_ptr<Chunk> whatChunkHere(Vec2<float> position) { return std::weak_ptr<Chunk>(); } // Bad value. Just to compile
};
