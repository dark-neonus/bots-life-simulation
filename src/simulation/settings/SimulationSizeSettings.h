#pragma once

struct SimulationSizeSettings
{
    int unit;
    int numberOfChunksX;
    int numberOfChunksY;
    int unitsPerChunk;

    /// @brief Constructs SimulationSizeSettings with default or provided values for all members.
    /// @param unit_ Base unit size for simulation (default: 1).
    /// @param numberOfChunksX_ Number of chunks along the X-axis (default: 10).
    /// @param numberOfChunksY_ Number of chunks along the Y-axis (default: 10).
    SimulationSizeSettings(
        int unit_ = 10,
        int numberOfChunksX_ = 10,
        int numberOfChunksY_ = 10,
        int unitsPerChunk_ = 10)
        : unit(unit_),
          numberOfChunksX(numberOfChunksX_),
          numberOfChunksY(numberOfChunksY_),
          unitsPerChunk(unitsPerChunk_) {}
};