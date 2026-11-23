#include "RoadRenderer.h"

#include <cmath>
#include "core/Settings.h"
#include "raylib.h"

namespace world
{
void DrawRoadSegment(float segmentZ, std::size_t segmentIndex)
{
    const float halfTrack = Settings::TRACK_WIDTH * 0.5F;
    constexpr float sidewalkWidth = 2.6F;
    const float sidewalkLeftX = -halfTrack - sidewalkWidth * 0.5F;
    const float sidewalkRightX = halfTrack + sidewalkWidth * 0.5F;

    DrawCube({0.0F, -0.16F, segmentZ}, Settings::TRACK_WIDTH, 0.32F,
             Settings::TRACK_SEGMENT_LENGTH, Color{44, 48, 56, 255});

    for (int marker = 0; marker < 2; ++marker)
    {
        const float laneMarkerX = (marker == 0)
            ? -Settings::LANE_WIDTH * 0.5F
            :  Settings::LANE_WIDTH * 0.5F;
        for (int k = -1; k <= 1; ++k)
        {
            const float dashZ = segmentZ + static_cast<float>(k) * 3.8F;
            DrawCube({laneMarkerX, 0.02F, dashZ}, 0.10F, 0.02F, 2.2F, RAYWHITE);
        }
    }

    for (int k = -2; k <= 2; ++k)
    {
        const float curbZ = segmentZ + static_cast<float>(k) * 2.4F;
        const Color curbColor = (std::abs(k + static_cast<int>(segmentIndex)) % 2 == 0)
            ? Color{245, 195, 35, 255}
            : Color{35, 35, 40, 255};
        DrawCube({-halfTrack, 0.06F, curbZ}, 0.18F, 0.12F, 2.4F, curbColor);
        DrawCube({halfTrack, 0.06F, curbZ}, 0.18F, 0.12F, 2.4F, curbColor);
    }

    DrawCube({sidewalkLeftX, -0.06F, segmentZ}, sidewalkWidth, 0.24F,
             Settings::TRACK_SEGMENT_LENGTH, Color{215, 218, 222, 255});
    DrawCube({sidewalkRightX, -0.06F, segmentZ}, sidewalkWidth, 0.24F,
             Settings::TRACK_SEGMENT_LENGTH, Color{215, 218, 222, 255});

    for (int k = -1; k <= 1; ++k)
    {
        const float seamZ = segmentZ + static_cast<float>(k) * 4.0F;
        DrawCube({sidewalkLeftX, 0.07F, seamZ}, sidewalkWidth, 0.02F,
                 0.06F, Color{165, 168, 175, 255});
        DrawCube({sidewalkRightX, 0.07F, seamZ}, sidewalkWidth, 0.02F,
                 0.06F, Color{165, 168, 175, 255});
    }
}
}
