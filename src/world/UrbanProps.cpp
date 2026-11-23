#include "UrbanProps.h"

namespace world::props
{
void DrawLowBuilding(Vector3 position, float width, float height, float depth, Color color)
{
    DrawCube(position, width, height, depth, color);
    DrawCube({position.x, position.y + height * 0.5F + 0.18F, position.z},
             width + 0.20F, 0.36F, depth + 0.20F, Color{78, 82, 88, 255});
    DrawCube({position.x, 0.82F, position.z - depth * 0.52F},
             width * 0.24F, 1.50F, 0.08F, Color{68, 48, 38, 255});
}

void DrawBuildingDoor(float faceX, float baseZ, float height, bool leftFacade,
                      unsigned int seed)
{
    (void)height;
    const bool commercial = (seed % 3U) == 0U;
    const float doorWidth = commercial ? 1.30F : 1.10F;
    const float doorHeight = commercial ? 2.20F : 1.85F;
    const float doorX = faceX + (leftFacade ? 0.055F : -0.055F);

    const Color frameColor = commercial
        ? Color{70, 82, 92, 255}
        : Color{92, 62, 42, 255};
    const Color doorColor = commercial
        ? Color{95, 150, 175, 255}
        : Color{125, 82, 48, 255};

    DrawCube({doorX, doorHeight * 0.5F, baseZ},
             0.10F, doorHeight, doorWidth, doorColor);
    DrawCube({doorX, doorHeight * 0.5F, baseZ - doorWidth * 0.5F - 0.04F},
             0.12F, doorHeight + 0.18F, 0.08F, frameColor);
    DrawCube({doorX, doorHeight * 0.5F, baseZ + doorWidth * 0.5F + 0.04F},
             0.12F, doorHeight + 0.18F, 0.08F, frameColor);
    DrawCube({doorX, doorHeight + 0.10F, baseZ},
             0.12F, 0.10F, doorWidth + 0.16F, frameColor);

    DrawCube({doorX + (leftFacade ? -0.055F : 0.055F),
              1.05F,
              baseZ + (leftFacade ? 0.28F : -0.28F)},
             0.04F, 0.06F, 0.10F, Color{245, 205, 95, 255});

    if (commercial)
    {
        DrawCube({doorX, 1.35F, baseZ},
                 0.14F, 0.08F, doorWidth * 0.82F,
                 Color{210, 225, 230, 255});
    }
}
}
