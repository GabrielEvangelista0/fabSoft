#pragma once

#include "raylib.h"

namespace world::props
{
void DrawLowBuilding(Vector3 position, float width, float height, float depth, Color color);
void DrawBuildingDoor(float faceX, float baseZ, float height, bool leftFacade,
                      unsigned int seed);
}
