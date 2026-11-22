#pragma once

#include <array>

#include "core/Settings.h"

struct TrackSpawner
{
    std::array<float, Settings::TRACK_SEGMENT_COUNT> segmentZ{};
};

void InitializeTrack(TrackSpawner& track);
void UpdateTrack(TrackSpawner& track, float deltaTime, float speed);
void DrawTrack(const TrackSpawner& track);
