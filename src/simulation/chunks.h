#pragma once

#include <memory>
#include <vector>
#include <unordered_set>
#include <stdexcept>

#include "utilities/utilities.h"
#include "simulation.h"

namespace std {
    template <typename T>
    struct hash<std::weak_ptr<T>> {
        size_t operator()(const std::weak_ptr<T>& wp) const {
            auto shared = wp.lock();
            return shared ? reinterpret_cast<size_t>(shared.get()) : 0;
        }
    };

    template <typename T>
    struct equal_to<std::weak_ptr<T>> {
        bool operator()(const std::weak_ptr<T>& wp1, const std::weak_ptr<T>& wp2) const {
            return wp1.lock() == wp2.lock();
        }
    };
}

class SimulationObject;
class Simulation;

class Chunk : public std::enable_shared_from_this<Chunk>
{
private:
public:
    const Vec2<float> startPos;
    const Vec2<float> endPos;
    const float chunkSize;

    std::unordered_set<std::weak_ptr<SimulationObject>,
        std::hash<std::weak_ptr<SimulationObject>>, std::equal_to<std::weak_ptr<SimulationObject>>> objects;

    Chunk(float startX, float startY, float chunkSize_)
        : startPos(startX, startY), chunkSize(chunkSize_), endPos(startX + chunkSize, startY + chunkSize)
    {
    }

    /// @brief Add given object to objects vector and assigne self to objects chunk property
    /// @param obj Object to
    template <typename T>
    void addObject(std::shared_ptr<T> obj) {
        obj->setChunk(shared_from_this());
        objects.insert(obj);
    }

    /// @brief Move object from current chunk to another
    /// @param objectToMove Object of current chunk to move
    /// @param destinationChunk Chunk for moving given object to
    template <typename T>
    void moveToChunk(std::shared_ptr<T> objectToMove, std::shared_ptr<Chunk> destinationChunk) {
        destinationChunk->addObject(objectToMove);
        objectToMove->setChunk(destinationChunk);
        objects.erase(objectToMove);
    }

    /// @brief Check if given object in this chunk. Use to check if object gone to another chunk after movement
    /// @param obj Object to check
    /// @return true if object in this cunk, false otherwise
    bool isObjectInChunk(std::shared_ptr<SimulationObject> obj) {
        return objects.find(obj) != objects.end();
    }
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
    std::shared_ptr<Chunk> getChunk(int xIndex, int yIndex) {
        if (xIndex >= 0 && xIndex < numberOfChunksX && yIndex >= 0 && yIndex < numberOfChunksY) {
            return std::make_shared<Chunk>(chunks[yIndex][xIndex]);
        }

        return nullptr;
    }

    /// @brief Get and return chunk which own given position
    /// @param position Position to check
    /// @return Return chunk which contain given position. If there is no such chunk, return std::weak_ptr<Chunk>() meaning nullptr
    std::shared_ptr<Chunk> whatChunkHere(Vec2<float> position) {
        int xIndex = static_cast<int>(position.x / chunkSize);
        int yIndex = static_cast<int>(position.y / chunkSize);

        return getChunk(xIndex, yIndex);
    } 
};
