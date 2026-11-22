#include "TrackSpawner.h"

#include <cmath>
#include "core/Settings.h"
#include "raylib.h"

void InitializeTrack(TrackSpawner& track)
{
    for (int i = 0; i < Settings::TRACK_SEGMENT_COUNT; ++i)
    {
        track.segmentZ[static_cast<std::size_t>(i)] =
            (static_cast<float>(i) - 0.5F) * Settings::TRACK_SEGMENT_LENGTH;
    }
}

void UpdateTrack(TrackSpawner& track, float deltaTime, float speed)
{
    float furthestSegmentZ = track.segmentZ[0];
    for (float& segmentZ : track.segmentZ)
    {
        segmentZ -= speed * deltaTime;
        furthestSegmentZ = segmentZ > furthestSegmentZ ? segmentZ : furthestSegmentZ;
    }

    for (float& segmentZ : track.segmentZ)
    {
        if (segmentZ < -Settings::TRACK_SEGMENT_LENGTH)
        {
            segmentZ = furthestSegmentZ + Settings::TRACK_SEGMENT_LENGTH;
            furthestSegmentZ = segmentZ;
        }
    }
}

void DrawTrack(const TrackSpawner& track)
{
    // Elementos distantes no horizonte (Sol e montanhas de fundo)
    DrawSphere({20.0F, 28.0F, 75.0F}, 6.0F, GOLD);
    DrawSphere({-30.0F, 6.0F, 85.0F}, 22.0F, Color{70, 110, 80, 255});
    DrawSphere({35.0F, 8.0F, 90.0F}, 25.0F, Color{60, 100, 75, 255});
    DrawSphere({0.0F, 10.0F, 95.0F}, 28.0F, Color{50, 90, 70, 255});

    for (size_t idx = 0; idx < track.segmentZ.size(); ++idx)
    {
        const float segmentZ = track.segmentZ[idx];

        // Grama lateral expansiva (esquerda e direita)
        DrawCube({-20.0F, -0.22F, segmentZ}, 24.0F, 0.20F, Settings::TRACK_SEGMENT_LENGTH, Color{45, 135, 55, 255});
        DrawCube({20.0F, -0.22F, segmentZ}, 24.0F, 0.20F, Settings::TRACK_SEGMENT_LENGTH, Color{45, 135, 55, 255});

        // Calçadas de concreto nas margens da pista
        const float halfTrack = Settings::TRACK_WIDTH * 0.5F;
        DrawCube({-halfTrack - 0.7F, -0.10F, segmentZ}, 1.4F, 0.28F, Settings::TRACK_SEGMENT_LENGTH, LIGHTGRAY);
        DrawCube({halfTrack + 0.7F, -0.10F, segmentZ}, 1.4F, 0.28F, Settings::TRACK_SEGMENT_LENGTH, LIGHTGRAY);

        // Guias/meio-fio com textura zebrada/destacada
        DrawCube({-halfTrack, 0.05F, segmentZ}, 0.15F, 0.10F, Settings::TRACK_SEGMENT_LENGTH, YELLOW);
        DrawCube({halfTrack, 0.05F, segmentZ}, 0.15F, 0.10F, Settings::TRACK_SEGMENT_LENGTH, YELLOW);

        // Asfalto da pista principal
        DrawCube(
            {0.0F, -0.16F, segmentZ},
            Settings::TRACK_WIDTH,
            0.32F,
            Settings::TRACK_SEGMENT_LENGTH,
            Color{40, 42, 45, 255});

        // Linhas tracejadas brancas divisórias das 3 raias
        for (const float laneMarkerX : {-Settings::LANE_WIDTH * 0.5F, Settings::LANE_WIDTH * 0.5F})
        {
            // Tracejado: três segmentos por bloco de pista
            for (int k = -1; k <= 1; ++k)
            {
                const float dashZ = segmentZ + static_cast<float>(k) * 3.5F;
                DrawCube(
                    {laneMarkerX, 0.02F, dashZ},
                    0.08F,
                    0.02F,
                    2.0F,
                    RAYWHITE);
            }
        }

        // Cenário lateral móvel (Árvores e Postes de Luz alternados)
        if (idx % 2 == 0)
        {
            // Árvore na margem esquerda
            const Vector3 treeBase{-halfTrack - 2.2F, 0.0F, segmentZ};
            DrawCylinder(treeBase, 0.22F, 0.28F, 1.8F, 8, Color{110, 65, 30, 255});
            DrawSphere({treeBase.x, 2.2F, treeBase.z}, 1.1F, Color{34, 139, 34, 255});
            DrawSphere({treeBase.x, 3.0F, treeBase.z}, 0.8F, Color{46, 160, 46, 255});
        }
        else
        {
            // Poste de iluminação na margem direita
            const Vector3 poleBase{halfTrack + 1.8F, 0.0F, segmentZ};
            DrawCylinder(poleBase, 0.08F, 0.10F, 3.2F, 6, DARKGRAY);
            // Haste horizontal
            DrawCube({poleBase.x - 0.4F, 3.2F, poleBase.z}, 0.8F, 0.08F, 0.08F, DARKGRAY);
            // Lâmpada e domo luminoso
            DrawSphere({poleBase.x - 0.8F, 3.15F, poleBase.z}, 0.18F, Color{255, 255, 180, 255});
        }
    }
}
