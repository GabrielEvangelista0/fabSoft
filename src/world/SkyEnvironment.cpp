#include "SkyEnvironment.h"

#include <cmath>
#include "raylib.h"

namespace world
{
void DrawSkyEnvironment(float worldZ)
{
    const float time = static_cast<float>(GetTime());
    DrawSphere({24.0F, 30.0F, worldZ + 86.0F}, 5.5F, Color{255, 230, 80, 255});
    DrawSphereWires({24.0F, 30.0F, worldZ + 86.0F}, 6.5F, 10, 10, Color{255, 245, 140, 160});

    const float cloudOffset = std::fmod(time * 1.4F, 60.0F);
    for (float cz = 15.0F; cz <= 85.0F; cz += 28.0F)
    {
        const float cloudZ = worldZ + cz - cloudOffset + 10.0F;
        DrawSphere({-19.0F, 24.0F, cloudZ}, 3.2F, Color{250, 252, 255, 235});
        DrawSphere({-16.0F, 24.6F, cloudZ}, 4.2F, Color{255, 255, 255, 245});
        DrawSphere({-12.8F, 24.0F, cloudZ}, 3.0F, Color{250, 252, 255, 235});
        DrawSphere({16.0F, 26.0F, cloudZ + 14.0F}, 3.5F, Color{250, 252, 255, 235});
        DrawSphere({19.5F, 26.7F, cloudZ + 14.0F}, 4.4F, Color{255, 255, 255, 245});
        DrawSphere({23.2F, 26.0F, cloudZ + 14.0F}, 3.2F, Color{250, 252, 255, 235});
    }

    DrawCube({-32.0F, 12.0F, worldZ + 96.0F}, 14.0F, 26.0F, 4.0F, Color{120, 165, 210, 255});
    DrawCube({-16.0F, 16.0F, worldZ + 98.0F}, 12.0F, 34.0F, 4.0F, Color{110, 155, 200, 255});
    DrawCube({0.0F, 20.0F, worldZ + 100.0F}, 16.0F, 42.0F, 4.0F, Color{100, 145, 190, 255});
    DrawCube({18.0F, 14.0F, worldZ + 97.0F}, 14.0F, 30.0F, 4.0F, Color{115, 160, 205, 255});
    DrawCube({34.0F, 11.0F, worldZ + 95.0F}, 12.0F, 24.0F, 4.0F, Color{120, 165, 210, 255});
}
}
