#include "TrackSpawner.h"

#include <algorithm>
#include <cmath>

#include "core/Settings.h"
#include "raylib.h"
#include "world/BuildingSpawner.h"
#include "world/RoadPropsRenderer.h"
#include "world/RoadRenderer.h"
#include "world/SkyEnvironment.h"
#include "world/UrbanProps.h"

void InitializeTrack(TrackSpawner& track)
{
    track.worldZ = 0.0F;
    if (track.resourcesLoaded)
    {
        UnloadTrack(track);
    }

    world::building::InitializeResources(track);
    track.resourcesLoaded = true;

    for (int i = 0; i < Settings::TRACK_SEGMENT_COUNT; ++i)
    {
        track.segmentZ[static_cast<std::size_t>(i)] =
            (static_cast<float>(i) - 0.5F) * Settings::TRACK_SEGMENT_LENGTH;
        track.chunkSeed[static_cast<std::size_t>(i)] =
            world::building::HashLotIndex(
                world::building::GetLotIndex(track.segmentZ[static_cast<std::size_t>(i)]));
        world::building::RefreshLotInstance(track, static_cast<std::size_t>(i));
    }
}

void UnloadTrack(TrackSpawner& track)
{
    if (!track.resourcesLoaded)
    {
        return;
    }
    world::building::UnloadResources(track);
    track.resourcesLoaded = false;
}

void UpdateTrack(TrackSpawner& track, float deltaTime, float speed, float worldZ)
{
    (void)deltaTime;
    (void)speed;
    track.worldZ = worldZ;

    float furthestSegmentZ = worldZ - 1000.0F;
    for (float z : track.segmentZ)
    {
        furthestSegmentZ = std::max(furthestSegmentZ, z);
    }

    for (std::size_t i = 0; i < track.segmentZ.size(); ++i)
    {
        float& segmentZ = track.segmentZ[i];
        if (segmentZ < worldZ - Settings::TRACK_SEGMENT_LENGTH)
        {
            segmentZ = furthestSegmentZ + Settings::TRACK_SEGMENT_LENGTH;
            furthestSegmentZ = segmentZ;
        }
        track.chunkSeed[i] = world::building::HashLotIndex(
            world::building::GetLotIndex(segmentZ));
        world::building::RefreshLotInstance(track, i);
    }
}

void DrawTrack(const TrackSpawner& track)
{
    world::DrawSkyEnvironment(track.worldZ);

    const Color buildingColorsLeft[] = {
        Color{225, 138, 108, 255},
        Color{115, 155, 195, 255},
        Color{230, 198, 142, 255},
        Color{155, 188, 138, 255},
        Color{210, 130, 145, 255}};
    const Color buildingColorsRight[] = {
        Color{135, 192, 155, 255},
        Color{230, 148, 118, 255},
        Color{125, 162, 202, 255},
        Color{238, 208, 138, 255},
        Color{178, 148, 192, 255}};

    for (std::size_t idx = 0; idx < track.segmentZ.size(); ++idx)
    {
        const float segmentZ = track.segmentZ[idx];
        if (segmentZ < track.worldZ - Settings::TRACK_SEGMENT_LENGTH ||
            segmentZ > track.worldZ + 150.0F)
        {
            continue;
        }

        const unsigned int chunkSeed = track.chunkSeed[idx];
        const int leftArchetype = track.leftLots[idx].archetype;
        const int rightArchetype = track.rightLots[idx].archetype;

        world::DrawRoadSegment(segmentZ, idx);
        world::DrawRoadProps(static_cast<int>(chunkSeed % 4U), segmentZ);

        if (!track.showBuildings)
        {
            continue;
        }

        const float bldgDepth = Settings::TRACK_SEGMENT_LENGTH;
        const unsigned int lotType = chunkSeed % 5U;
        const BuildingLot& leftLot = track.leftLots[idx];
        const BuildingLot& rightLot = track.rightLots[idx];

        // Continuous strip between the sidewalk and building face removes
        // the remaining lateral gap for every lot/low-building variant.
        DrawCube(
            {-8.65F, 0.02F, segmentZ},
            3.90F,
            0.04F,
            bldgDepth,
            Color{205, 200, 186, 255});
        DrawCube(
            {8.65F, 0.02F, segmentZ},
            3.90F,
            0.04F,
            bldgDepth,
            Color{205, 200, 186, 255});

        DrawCube(
            {leftLot.backX, leftLot.backHeight * 0.5F, segmentZ},
            leftLot.backWidth,
            leftLot.backHeight,
            bldgDepth,
            buildingColorsLeft[(chunkSeed + 1U) % 5U]);
        DrawCube(
            {rightLot.backX, rightLot.backHeight * 0.5F, segmentZ},
            rightLot.backWidth,
            rightLot.backHeight,
            bldgDepth,
            buildingColorsRight[(chunkSeed + 3U) % 5U]);

        const float leftHeight = (lotType == 4U) ? 5.20F : leftLot.height;
        if (lotType == 4U)
        {
            world::props::DrawLowBuilding(
                {leftLot.x, leftHeight * 0.5F, segmentZ},
                leftLot.width, leftHeight, bldgDepth, leftLot.color);
        }
        else
        {
            DrawModelEx(
                track.buildingModels[static_cast<std::size_t>(leftArchetype)],
                {leftLot.x, leftHeight * 0.5F, segmentZ},
                {0.0F, 1.0F, 0.0F}, 0.0F,
                {leftLot.width, leftHeight, bldgDepth},
                leftLot.color);
            DrawCubeWires(
                {leftLot.x, leftHeight * 0.5F, segmentZ},
                leftLot.width, leftHeight, bldgDepth, Fade(BLACK, 0.20F));
        }
        world::props::DrawBuildingDoor(
            leftLot.x + leftLot.width * 0.5F,
            segmentZ, leftHeight, true, chunkSeed);

        const unsigned int rightLotType = (lotType + 2U) % 5U;
        const float rightHeight = (rightLotType == 4U) ? 5.20F : rightLot.height;
        if (rightLotType == 4U)
        {
            world::props::DrawLowBuilding(
                {rightLot.x, rightHeight * 0.5F, segmentZ},
                rightLot.width, rightHeight, bldgDepth, rightLot.color);
        }
        else
        {
            DrawModelEx(
                track.buildingModels[static_cast<std::size_t>(rightArchetype)],
                {rightLot.x, rightHeight * 0.5F, segmentZ},
                {0.0F, 1.0F, 0.0F}, 0.0F,
                {rightLot.width, rightHeight, bldgDepth},
                rightLot.color);
            DrawCubeWires(
                {rightLot.x, rightHeight * 0.5F, segmentZ},
                rightLot.width, rightHeight, bldgDepth, Fade(BLACK, 0.20F));
        }
        world::props::DrawBuildingDoor(
            rightLot.x - rightLot.width * 0.5F,
            segmentZ, rightHeight, false, chunkSeed + 2U);
    }
}
