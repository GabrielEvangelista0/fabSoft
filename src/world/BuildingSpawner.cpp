#include "BuildingSpawner.h"

#include <array>
#include <cmath>

#include "core/Settings.h"
#include "raylib.h"
#include "world/TrackSpawner.h"

namespace world::building
{
unsigned int HashLotIndex(int lotIndex)
{
    unsigned int seed = static_cast<unsigned int>(lotIndex);
    seed ^= seed >> 16U;
    seed *= 0x7FEB352DU;
    seed ^= seed >> 15U;
    seed *= 0x846CA68BU;
    seed ^= seed >> 16U;
    return seed;
}

int GetLotIndex(float worldZ)
{
    return static_cast<int>(std::floor(
        (worldZ + Settings::TRACK_SEGMENT_LENGTH * 0.5F)
        / Settings::TRACK_SEGMENT_LENGTH));
}

void RefreshLotInstance(TrackSpawner& track, std::size_t index)
{
    const unsigned int seed = track.chunkSeed[index];
    constexpr float halfTrack = Settings::TRACK_WIDTH * 0.5F;
    constexpr float sidewalkWidth = 2.6F;
    constexpr float buildingSetback = 0.0F;
    const float curbOuterLeft = -halfTrack - sidewalkWidth;
    const float curbOuterRight = halfTrack + sidewalkWidth;

    const Color leftPalette[] = {
        {225, 138, 108, 255}, {115, 155, 195, 255},
        {230, 198, 142, 255}, {155, 188, 138, 255},
        {210, 130, 145, 255}};
    const Color rightPalette[] = {
        {135, 192, 155, 255}, {230, 148, 118, 255},
        {125, 162, 202, 255}, {238, 208, 138, 255},
        {178, 148, 192, 255}};

    BuildingLot& left = track.leftLots[index];
    left.width = 6.20F + static_cast<float>(seed % 4U) * 0.20F;
    left.height = 9.0F + static_cast<float>(seed % 9U)
        + ((seed + 1U) % 4U == 2U ? 2.5F : 0.0F);
    left.depth = Settings::TRACK_SEGMENT_LENGTH;
    left.x = curbOuterLeft - buildingSetback - left.width * 0.5F;
    left.backWidth = 6.20F;
    left.backHeight = 7.5F + static_cast<float>(seed % 6U);
    left.backX = left.x - left.width * 0.5F - left.backWidth * 0.5F;
    left.archetype = static_cast<int>((seed + 1U) % 4U);
    left.lotType = seed % 5U;
    left.color = leftPalette[seed % 5U];

    BuildingLot& right = track.rightLots[index];
    right.width = 6.20F + static_cast<float>((seed + 2U) % 4U) * 0.20F;
    right.height = 8.5F + static_cast<float>((seed * 5U) % 11U)
        + ((seed + 3U) % 4U == 2U ? 3.0F : 0.0F);
    right.depth = Settings::TRACK_SEGMENT_LENGTH;
    right.x = curbOuterRight + buildingSetback + right.width * 0.5F;
    right.backWidth = 6.20F;
    right.backHeight = 7.0F + static_cast<float>((seed * 3U) % 8U);
    right.backX = right.x + right.width * 0.5F + right.backWidth * 0.5F;
    right.archetype = static_cast<int>((seed + 3U) % 4U);
    right.lotType = (seed % 5U + 2U) % 5U;
    right.color = rightPalette[(seed + 2U) % 5U];
}

namespace
{
Texture2D GenerateFacadeTexture(int archetype, Color baseColor)
{
    constexpr int textureWidth = 256;
    constexpr int textureHeight = 512;
    Image image = GenImageColor(textureWidth, textureHeight, baseColor);

    const Color frame{48, 50, 54, 255};
    const Color windowWarm{255, 225, 150, 255};
    const Color windowBlue{150, 205, 230, 255};
    const Color trim{235, 224, 198, 255};

    if (archetype == 0)
    {
        ImageDrawRectangle(&image, 0, 0, textureWidth, 112, trim);
        for (int wx = 18; wx < textureWidth; wx += 82)
        {
            ImageDrawRectangle(&image, wx, 18, 58, 76, frame);
            ImageDrawRectangle(&image, wx + 5, 23, 48, 66, windowBlue);
        }
        for (int row = 0; row < 5; ++row)
        {
            const int wy = 132 + row * 72;
            for (int column = 0; column < 3; ++column)
            {
                const int wx = 20 + column * 82;
                ImageDrawRectangle(&image, wx, wy, 58, 48, frame);
                ImageDrawRectangle(&image, wx + 5, wy + 5, 48, 38,
                    ((row + column) % 3 == 0) ? windowWarm : windowBlue);
            }
        }
    }
    else if (archetype == 1)
    {
        for (int row = 0; row < 6; ++row)
        {
            const int wy = 24 + row * 78;
            for (int column = 0; column < 3; ++column)
            {
                const int wx = 22 + column * 78;
                ImageDrawRectangle(&image, wx, wy, 54, 58, frame);
                ImageDrawRectangle(&image, wx + 5, wy + 5, 44, 48,
                    ((row + column) % 4 == 1) ? windowBlue : windowWarm);
                if (row > 1)
                {
                    ImageDrawRectangle(&image, wx - 10, wy + 51, 74, 8, trim);
                }
            }
        }
    }
    else
    {
        const bool modern = archetype == 2;
        const Color featureColor = modern ? Color{75, 105, 125, 255}
                                          : Color{118, 72, 58, 255};
        for (int y = 112; y < 430; y += modern ? 48 : 58)
        {
            const int offset = ((y / 48) % 2) * 10;
            for (int x = 18 - offset; x < textureWidth - 10; x += modern ? 62 : 58)
            {
                ImageDrawRectangle(&image, x, y, modern ? 48 : 44,
                                   modern ? 34 : 42, frame);
                ImageDrawRectangle(&image, x + 5, y + 5,
                                   modern ? 38 : 34,
                                   modern ? 24 : 32,
                                   ((y + x) % 3 == 0) ? windowWarm : windowBlue);
                if (!modern)
                {
                    ImageDrawRectangle(&image, x - 4, y + 44,
                                       modern ? 56 : 52, 5, featureColor);
                }
            }
        }
    }

    ImageDrawRectangle(&image, 0, 430, textureWidth, textureHeight - 430, trim);
    for (int x = 18; x < textureWidth - 10; x += 62)
    {
        if (x >= 96 && x <= 160)
        {
            continue;
        }
        ImageDrawRectangle(&image, x, 448, 44, 50, frame);
        ImageDrawRectangle(&image, x + 5, 453, 34, 40,
                           ((x / 62) % 2 == 0) ? windowWarm : windowBlue);
    }

    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);
    SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);
    return texture;
}
}

void InitializeResources(TrackSpawner& track)
{
    const Color palettes[] = {
        {205, 153, 112, 255}, {128, 167, 196, 255},
        {92, 122, 142, 255}, {154, 82, 62, 255}};
    for (int archetype = 0; archetype < 4; ++archetype)
    {
        track.buildingTextures[static_cast<std::size_t>(archetype)] =
            GenerateFacadeTexture(archetype, palettes[archetype]);
        Mesh mesh = GenMeshCube(1.0F, 1.0F, 1.0F);
        track.buildingModels[static_cast<std::size_t>(archetype)] = LoadModelFromMesh(mesh);
        SetMaterialTexture(
            &track.buildingModels[static_cast<std::size_t>(archetype)].materials[0],
            MATERIAL_MAP_DIFFUSE,
            track.buildingTextures[static_cast<std::size_t>(archetype)]);
    }
}

void UnloadResources(TrackSpawner& track)
{
    for (int i = 0; i < 4; ++i)
    {
        UnloadModel(track.buildingModels[static_cast<std::size_t>(i)]);
        UnloadTexture(track.buildingTextures[static_cast<std::size_t>(i)]);
    }
}
}
