#pragma once

#include <cstddef>

struct TrackSpawner;

namespace world::building
{
unsigned int HashLotIndex(int lotIndex);
int GetLotIndex(float worldZ);
void InitializeResources(TrackSpawner& track);
void UnloadResources(TrackSpawner& track);
void RefreshLotInstance(TrackSpawner& track, std::size_t index);
}
