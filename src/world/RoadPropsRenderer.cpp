#include "RoadPropsRenderer.h"

#include "core/Settings.h"

namespace world
{
void DrawRoadProps(int propType, float segmentZ)
{
    const float halfTrack = Settings::TRACK_WIDTH * 0.5F;
    constexpr float sidewalkWidth = 2.6F;
    const float sidewalkLeftX = -halfTrack - sidewalkWidth * 0.5F;
    const float sidewalkRightX = halfTrack + sidewalkWidth * 0.5F;

    if (propType == 0)
    {
        const Vector3 planterPos{sidewalkLeftX, 0.16F, segmentZ - 2.0F};
        DrawCube(planterPos, 0.70F, 0.24F, 1.4F, Color{140, 142, 148, 255});
        DrawCube({planterPos.x, planterPos.y + 0.10F, planterPos.z}, 0.60F, 0.10F, 1.3F, Color{75, 48, 25, 255});
        DrawSphere({planterPos.x, planterPos.y + 0.25F, planterPos.z - 0.35F}, 0.28F, Color{60, 180, 85, 255});
        DrawSphere({planterPos.x, planterPos.y + 0.28F, planterPos.z}, 0.30F, Color{45, 165, 70, 255});
        DrawSphere({planterPos.x, planterPos.y + 0.25F, planterPos.z + 0.35F}, 0.28F, Color{60, 180, 85, 255});

        const Vector3 lampBase{sidewalkRightX + 0.4F, 0.0F, segmentZ + 2.0F};
        DrawCylinder(lampBase, 0.09F, 0.12F, 3.6F, 8, Color{50, 55, 62, 255});
        DrawCube({lampBase.x - 0.45F, 3.6F, lampBase.z}, 0.9F, 0.09F, 0.09F, Color{50, 55, 62, 255});
        DrawSphere({lampBase.x - 0.85F, 3.55F, lampBase.z}, 0.20F, Color{255, 248, 190, 255});
    }
    else if (propType == 1)
    {
        const Vector3 treeBase{sidewalkLeftX - 0.3F, 0.0F, segmentZ + 1.0F};
        DrawCylinder(treeBase, 0.20F, 0.26F, 2.0F, 8, Color{115, 75, 42, 255});
        DrawSphere({treeBase.x, 2.5F, treeBase.z}, 1.2F, Color{55, 175, 80, 255});
        DrawSphere({treeBase.x, 3.3F, treeBase.z}, 0.9F, Color{70, 195, 95, 255});
        DrawSphere({treeBase.x, 4.0F, treeBase.z}, 0.6F, Color{90, 215, 115, 255});
        DrawCylinder({treeBase.x + 0.2F, 0.065F, treeBase.z}, 0.9F, 0.9F, 0.01F, 12, Fade(BLACK, 0.18F));

        const Vector3 hydrantPos{sidewalkRightX + 0.5F, 0.0F, segmentZ - 2.5F};
        DrawCylinder(hydrantPos, 0.14F, 0.16F, 0.65F, 8, Color{225, 45, 40, 255});
        DrawSphere({hydrantPos.x, 0.65F, hydrantPos.z}, 0.14F, Color{225, 45, 40, 255});
        DrawCube({hydrantPos.x, 0.40F, hydrantPos.z}, 0.40F, 0.10F, 0.10F, Color{180, 30, 25, 255});
    }
    else if (propType == 2)
    {
        const Vector3 benchPos{sidewalkLeftX, 0.20F, segmentZ};
        DrawCube(benchPos, 0.50F, 0.08F, 1.30F, Color{160, 95, 45, 255});
        DrawCube({benchPos.x - 0.20F, benchPos.y + 0.25F, benchPos.z}, 0.08F, 0.45F, 1.30F, Color{160, 95, 45, 255});
        DrawCube({benchPos.x, benchPos.y - 0.10F, benchPos.z - 0.55F}, 0.45F, 0.20F, 0.08F, Color{45, 48, 52, 255});
        DrawCube({benchPos.x, benchPos.y - 0.10F, benchPos.z + 0.55F}, 0.45F, 0.20F, 0.08F, Color{45, 48, 52, 255});
    }
    else
    {
        const Vector3 binPos{sidewalkLeftX - 0.2F, 0.0F, segmentZ - 1.0F};
        DrawCylinder(binPos, 0.22F, 0.20F, 0.70F, 10, Color{70, 78, 88, 255});
        DrawCylinder({binPos.x, 0.70F, binPos.z}, 0.24F, 0.24F, 0.08F, 10, Color{45, 52, 60, 255});

        const Vector3 treeBase{sidewalkRightX + 0.4F, 0.0F, segmentZ + 1.5F};
        DrawCylinder(treeBase, 0.20F, 0.26F, 2.0F, 8, Color{115, 75, 42, 255});
        DrawSphere({treeBase.x, 2.5F, treeBase.z}, 1.2F, Color{55, 175, 80, 255});
        DrawSphere({treeBase.x, 3.3F, treeBase.z}, 0.9F, Color{70, 195, 95, 255});
        DrawSphere({treeBase.x, 4.0F, treeBase.z}, 0.6F, Color{90, 215, 115, 255});
    }
}
}
