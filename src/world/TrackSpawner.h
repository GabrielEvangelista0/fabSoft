#pragma once

#include <array>

#include "core/Settings.h"
#include "raylib.h"

struct BuildingLot
{
    float x = 0.0F;
    float width = 6.20F;
    float height = 9.0F;
    float depth = 12.0F;
    float backX = 0.0F;
    float backWidth = 6.20F;
    float backHeight = 8.0F;
    int archetype = 0;
    unsigned int lotType = 0U;
    Color color = WHITE;
};

struct TrackSpawner
{
    std::array<float, Settings::TRACK_SEGMENT_COUNT> segmentZ{};
    std::array<unsigned int, Settings::TRACK_SEGMENT_COUNT> chunkSeed{};
    std::array<Texture2D, 4> buildingTextures{};
    std::array<Model, 4> buildingModels{};
    bool resourcesLoaded = false;
    bool showBuildings = true;
    float worldZ = 0.0F;
    std::array<BuildingLot, Settings::TRACK_SEGMENT_COUNT> leftLots{};
    std::array<BuildingLot, Settings::TRACK_SEGMENT_COUNT> rightLots{};
};

void InitializeTrack(TrackSpawner& track);
void UnloadTrack(TrackSpawner& track);
void UpdateTrack(TrackSpawner& track, float deltaTime, float speed, float worldZ);
void DrawTrack(const TrackSpawner& track);
