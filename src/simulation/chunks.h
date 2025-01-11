#pragma once

#include "imgui.h"

#include <vector>
#include <stdexcept>
#include <algorithm>
#include <memory>
#include <unordered_set>

#include "utilities/utilities.h"
#include "simulation.h"
#include "objects/SimulationObject.h"

class SimulationObject;

using objectSet = std::unordered_set<std::weak_ptr<SimulationObject>,
    std::hash<std::weak_ptr<SimulationObject>>,
    std::equal_to<std::weak_ptr<SimulationObject>>>;

class Chunk : public std::enable_shared_from_this<Chunk>
{
private:
    RangeValue<float> seeDistanceMultiplier = RangeValue<float>(1.0f, 0.0f, 2.0f);
    RangeValue<float> speedMultiplier = RangeValue<float>(1.0f, 0.0f, 2.0f);
    RangeValue<float> hungryMultiplier = RangeValue<float>(1.0f, 0.0f, 2.0f);

    RangeValue<float> lostLifeChance = RangeValue<float>(0.0f, 0.0f, 1.0f);
    RangeValue<float> findFoodChance = RangeValue<float>(0.0f, 0.0f, 1.0f);
public:
    const int xIndex;
    const int yIndex;
    const float chunkSize;
    const Vec2<float> startPos;
    const Vec2<float> endPos;

    objectSet objects;

    Chunk(float startX, float startY, float chunkSize_, int xIndex_, int yIndex_)
        : xIndex(xIndex_), yIndex(yIndex_), chunkSize(chunkSize_),
        startPos(startX, startY), endPos(startX + chunkSize, startY + chunkSize)
    {
    }

    /// @brief Add given object to objects vector and assigne self to objects chunk property
    /// @param obj Object to
    template <typename T>
    void addObject(std::shared_ptr<T> obj) {
        obj->setChunk(shared_from_this());
        objects.insert(obj);
    }

    /// @brief Returns the set of all objects in the chunk.
    objectSet getObjects() const {
        return this->objects;
    }

    /// @brief Removes a specific SimulationObject from the list of objects in the simulation.
    void removeObject(std::shared_ptr<SimulationObject> obj);

    /// @brief Move object from current chunk to another
    /// @param objectToMove Object of current chunk to move
    /// @param destinationChunk Chunk for moving given object to
    template <typename T>
    void moveToChunk(std::shared_ptr<T> objectToMove, std::shared_ptr<Chunk> destinationChunk) {
        destinationChunk->addObject(objectToMove);
        objectToMove->setChunk(destinationChunk);
        objects.erase(objectToMove);
    }

    /// @brief Check if given object in chunks objects.
    /// @param obj Object to check
    /// @return true if object in this cunk, false otherwise
    bool isObjectInChunk(std::shared_ptr<SimulationObject> obj) {
        return objects.find(obj) != objects.end();
    }

    /// @brief Check if given position inside this chunk. Use to check if object gone to another chunk after movement
    /// @param obj Object to check
    /// @return true if object in this cunk, false otherwise
    bool isPosInsideChunk(Vec2<float> position) {
        return (startPos <= position && position <= endPos);
    }

    float getSeeDistanceMultiplier() { return seeDistanceMultiplier.get(); }
    float getSpeedMultiplier() { return speedMultiplier.get(); }
    float getHungryMultiplier() { return hungryMultiplier.get(); }
    float getLostLifeChance() { return lostLifeChance.get(); }
    float getFindFoodChance() { return findFoodChance.get(); }

    void displayInfo() {
        ImGui::SeparatorText("Chunk");
        ImGui::Text("Indexes:");
        ImGui::Text("xIndex: %i", xIndex);
        ImGui::Text("yIndex: %i", yIndex);
        ImGui::Separator();
        ImGui::Text("Coordinates:");
        ImGui::Text("StartPos: (%.1f, %.1f)", startPos.x, startPos.y);
        ImGui::Text("EndPos: (%.1f, %.1f)", endPos.x, endPos.y);
        ImGui::Separator();
        ImGui::Text("Effects:");
        ImGui::SliderFloat("SeeDistanceMultiplier", seeDistanceMultiplier.valuePointer(),
                            seeDistanceMultiplier.getMin(), seeDistanceMultiplier.getMax(), "%.2f");
        ImGui::SliderFloat("SpeedMultiplier", speedMultiplier.valuePointer(),
                            speedMultiplier.getMin(), speedMultiplier.getMax(), "%.2f");
        ImGui::SliderFloat("HungryMultiplier", hungryMultiplier.valuePointer(),
                            hungryMultiplier.getMin(), hungryMultiplier.getMax(), "%.2f");
        ImGui::SliderFloat("LostLifeChance", lostLifeChance.valuePointer(),
                            lostLifeChance.getMin(), lostLifeChance.getMax(), "%.2f");
        ImGui::SliderFloat("FindFoodChance", findFoodChance.valuePointer(),
                            findFoodChance.getMin(), findFoodChance.getMax(), "%.2f");
        
    }
};

class ChunkManager
{
private:

    std::vector<std::vector<std::shared_ptr<Chunk>>> chunks;
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
                chunks[y].emplace_back(std::make_shared<Chunk>(x * chunkSize, y * chunkSize, chunkSize, x, y));
            }
        }
    }

    /// @brief Return pointer to chunk from chunk matrix by coordinates (x, y)
    /// @param xIndex Column index of chunk
    /// @param yIndex Row index of chunk
    /// @return Return pointer to chunk at chunks[yIndex][xIndex]. If indexes are invalid, return nullptr
    std::shared_ptr<Chunk> getChunk(int xIndex, int yIndex) {
        if (xIndex >= 0 && xIndex < numberOfChunksX && yIndex >= 0 && yIndex < numberOfChunksY) {
            return chunks[yIndex][xIndex];
        }

        return nullptr;
    }

    /// @brief Get and return chunk which own given position
    /// @param position Position to check
    /// @return Return chunk which contain given position. If there is no such chunk, return nullptr
    std::shared_ptr<Chunk> whatChunkHere(Vec2<float> position) {
        // position.x - 1.0f fixes bug when object in the right bottom corner and chunkManger cant find chunk for this position
        int xIndex = static_cast<int>(std::max(0.0f, position.x - 1.0f) / chunkSize);
        int yIndex = static_cast<int>(std::max(0.0f, position.y - 1.0f) / chunkSize);

        return getChunk(xIndex, yIndex);
    }

    void drawChunksMesh(ImDrawList *draw_list, ImVec2 window_pos) {
        for (int y = 0; y < numberOfChunksY; y++)
        {
            for (int x = 0; x < numberOfChunksX; x++)
            {
                draw_list->AddRect(toImVec2(Vec2<float>(window_pos) + chunks[y][x]->startPos),
                                    toImVec2(Vec2<float>(window_pos) + chunks[y][x]->endPos), ImColor(colorInt(255, 255, 0, 10)), 0, 0, 2);
            }
        }
        // Draw map limits
        draw_list->AddRect(window_pos, ImVec2(window_pos.x + mapWidth, window_pos.y + mapHeight), ImColor(colorInt(255, 0, 255, 30)), 0, 0, 5);
    }

    // Updated ChunkIterator
    class ChunkIterator
    {
    private:
        const std::vector<std::vector<std::shared_ptr<Chunk>>> &chunks;
        size_t outerIndex, innerIndex;

    public:
        ChunkIterator(const std::vector<std::vector<std::shared_ptr<Chunk>>> &chunks_, size_t outerIdx, size_t innerIdx)
            : chunks(chunks_), outerIndex(outerIdx), innerIndex(innerIdx) {}

        // Dereference operator
        std::shared_ptr<Chunk> operator*() { return chunks[outerIndex][innerIndex]; }

        // Increment operator
        ChunkIterator &operator++()
        {
            if (++innerIndex >= chunks[outerIndex].size())
            {
                innerIndex = 0;
                ++outerIndex;
            }
            return *this;
        }

        // Equality operators
        bool operator==(const ChunkIterator &other) const
        {
            return &chunks == &other.chunks && outerIndex == other.outerIndex && innerIndex == other.innerIndex;
        }

        bool operator!=(const ChunkIterator &other) const { return !(*this == other); }
    };

    // Begin iterator
    ChunkIterator begin() const { return ChunkIterator(chunks, 0, 0); }

    // End iterator
    ChunkIterator end() const { return ChunkIterator(chunks, chunks.size(), 0); }

};
