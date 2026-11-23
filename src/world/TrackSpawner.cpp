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
    const float time = static_cast<float>(GetTime());

    // ==========================================
    // 1. CÉU GRADIENTE MANHÃ, SOL E NUVENS
    // ==========================================

    // Sol nascente suave com halos dourados difusos
    DrawSphere({18.0F, 26.0F, 85.0F}, 5.0F, Color{255, 230, 120, 255});
    DrawSphere({18.0F, 26.0F, 85.0F}, 7.2F, Fade(Color{255, 215,  80, 255}, 0.35F));
    DrawSphere({18.0F, 26.0F, 85.0F}, 9.5F, Fade(Color{255, 200, 100, 255}, 0.18F));

    // Nuvens pastel rosadas/lavanda flutuando suavemente
    const float cloudOffset = std::fmod(time * 1.2F, 55.0F);
    for (float cz = 20.0F; cz <= 80.0F; cz += 28.0F)
    {
        const float cloudZ = cz - cloudOffset + 10.0F;
        DrawSphere({-20.0F, 22.0F, cloudZ},        3.0F, Fade(Color{255, 220, 215, 255}, 0.80F));
        DrawSphere({-17.0F, 23.0F, cloudZ},        4.0F, Fade(Color{255, 230, 225, 255}, 0.85F));
        DrawSphere({-13.5F, 22.0F, cloudZ},        2.8F, Fade(Color{255, 215, 210, 255}, 0.78F));
        DrawSphere({ 16.0F, 24.0F, cloudZ + 12.0F}, 3.4F, Fade(Color{235, 220, 255, 255}, 0.80F));
        DrawSphere({ 20.0F, 25.0F, cloudZ + 12.0F}, 4.2F, Fade(Color{245, 235, 255, 255}, 0.85F));
        DrawSphere({ 24.0F, 24.0F, cloudZ + 12.0F}, 3.0F, Fade(Color{235, 225, 255, 255}, 0.78F));
    }

    // Silhueta metropolitana no horizonte — azul-ardósia suave
    DrawCube({-32.0F, 10.0F, 96.0F}, 13.0F, 22.0F, 5.0F, Color{ 90, 100, 118, 200});
    DrawCube({-16.0F, 14.0F, 98.0F}, 11.0F, 30.0F, 5.0F, Color{ 78,  88, 108, 200});
    DrawCube({  0.0F, 18.0F,100.0F}, 15.0F, 38.0F, 5.0F, Color{ 68,  78,  98, 200});
    DrawCube({ 18.0F, 12.0F, 97.0F}, 13.0F, 26.0F, 5.0F, Color{ 82,  92, 112, 200});
    DrawCube({ 34.0F,  9.0F, 95.0F}, 11.0F, 20.0F, 5.0F, Color{ 90, 100, 118, 200});

    // ==========================================
    // 2. SEGMENTOS DA PISTA E CIDADE
    // ==========================================
    const float halfTrack = Settings::TRACK_WIDTH * 0.5F;

    // Paleta harmonizada: tons desaturados/pastel urbano
    const Color buildingColorsLeft[] = {
        Color{188, 140, 110, 255},  // Terracota quente
        Color{100, 130, 165, 255},  // Azul-aço suave
        Color{175, 160, 130, 255},  // Bege urbano
        Color{165, 145,  80, 255},  // Mostarda suave
        Color{120, 115, 130, 255}   // Lavanda-ardósia
    };
    const Color buildingColorsRight[] = {
        Color{130, 155, 115, 255},  // Verde-oliva urbano
        Color{195, 145, 120, 255},  // Coral desbotado
        Color{ 90, 115, 145, 255},  // Marinho suave
        Color{195, 185, 160, 255},  // Areia clara
        Color{160, 120, 125, 255}   // Rosado desbotado
    };
    const Color windowFrameColor{55, 50, 45, 255};

    for (size_t idx = 0; idx < track.segmentZ.size(); ++idx)
    {
        const float segmentZ = track.segmentZ[idx];

        // --- ASFALTO ---
        DrawCube(
            {0.0F, -0.16F, segmentZ},
            Settings::TRACK_WIDTH, 0.32F, Settings::TRACK_SEGMENT_LENGTH,
            Color{42, 44, 50, 255});

        // --- LINHAS TRACEJADAS ---
        for (const float laneMarkerX : {-Settings::LANE_WIDTH * 0.5F, Settings::LANE_WIDTH * 0.5F})
        {
            for (int k = -1; k <= 1; ++k)
            {
                const float dashZ = segmentZ + static_cast<float>(k) * 3.8F;
                DrawCube({laneMarkerX, 0.02F, dashZ}, 0.10F, 0.02F, 2.2F, Color{235, 235, 230, 255});
            }
        }

        // --- MEIO-FIO ZEBRADO ---
        for (int k = -2; k <= 2; ++k)
        {
            const float curbZ = segmentZ + static_cast<float>(k) * 2.4F;
            const Color curbColor = (std::abs(k + static_cast<int>(idx)) % 2 == 0)
                ? Color{220, 175, 50, 255}
                : Color{30, 30, 30, 255};
            DrawCube({-halfTrack, 0.06F, curbZ}, 0.18F, 0.12F, 2.4F, curbColor);
            DrawCube({ halfTrack, 0.06F, curbZ}, 0.18F, 0.12F, 2.4F, curbColor);
        }

        // --- CALÇADAS ---
        constexpr float sidewalkWidth = 2.6F;
        const float sidewalkLeftX  = -halfTrack - (sidewalkWidth * 0.5F);
        const float sidewalkRightX =  halfTrack + (sidewalkWidth * 0.5F);

        DrawCube({sidewalkLeftX,  -0.06F, segmentZ}, sidewalkWidth, 0.24F, Settings::TRACK_SEGMENT_LENGTH, Color{205, 198, 185, 255});
        DrawCube({sidewalkRightX, -0.06F, segmentZ}, sidewalkWidth, 0.24F, Settings::TRACK_SEGMENT_LENGTH, Color{205, 198, 185, 255});

        // Juntas de concreto
        for (int k = -1; k <= 1; ++k)
        {
            const float seamZ = segmentZ + static_cast<float>(k) * 4.0F;
            DrawCube({sidewalkLeftX,  0.07F, seamZ}, sidewalkWidth, 0.02F, 0.06F, Color{145, 138, 128, 255});
            DrawCube({sidewalkRightX, 0.07F, seamZ}, sidewalkWidth, 0.02F, 0.06F, Color{145, 138, 128, 255});
        }

        // Sombra sutil dos prédios na calçada
        DrawCube({sidewalkLeftX,  0.001F, segmentZ}, sidewalkWidth * 0.6F, 0.001F, Settings::TRACK_SEGMENT_LENGTH * 0.9F, Fade(BLACK, 0.12F));
        DrawCube({sidewalkRightX, 0.001F, segmentZ}, sidewalkWidth * 0.6F, 0.001F, Settings::TRACK_SEGMENT_LENGTH * 0.9F, Fade(BLACK, 0.12F));

        // --- ELEMENTOS URBANOS ---
        if (idx % 2 == 0)
        {
            // Floreira
            const Vector3 planterPos{sidewalkLeftX, 0.16F, segmentZ - 2.0F};
            DrawCube(planterPos, 0.70F, 0.24F, 1.4F, Color{130, 120, 108, 255});
            DrawCube({planterPos.x, planterPos.y + 0.10F, planterPos.z}, 0.60F, 0.10F, 1.3F, Color{65, 45, 22, 255});
            DrawSphere({planterPos.x, planterPos.y + 0.25F, planterPos.z - 0.35F}, 0.28F, Color{ 95, 155,  80, 255});
            DrawSphere({planterPos.x, planterPos.y + 0.28F, planterPos.z},          0.30F, Color{ 80, 140,  68, 255});
            DrawSphere({planterPos.x, planterPos.y + 0.25F, planterPos.z + 0.35F}, 0.28F, Color{ 95, 155,  80, 255});
            DrawSphere({planterPos.x, planterPos.y + 0.40F, planterPos.z - 0.2F},  0.08F, Color{230, 115,  90, 255});
            DrawSphere({planterPos.x, planterPos.y + 0.42F, planterPos.z + 0.1F},  0.08F, Color{245, 215, 100, 255});
            DrawSphere({planterPos.x, planterPos.y + 0.40F, planterPos.z + 0.3F},  0.08F, Color{200, 150, 210, 255});

            // Poste vintage
            const Vector3 lampBase{sidewalkRightX + 0.4F, 0.0F, segmentZ + 2.0F};
            DrawCylinder(lampBase, 0.09F, 0.12F, 3.6F, 8, Color{55, 55, 58, 255});
            DrawCube({lampBase.x - 0.45F, 3.6F, lampBase.z}, 0.9F, 0.09F, 0.09F, Color{55, 55, 58, 255});
            DrawSphere({lampBase.x - 0.85F, 3.55F, lampBase.z}, 0.20F, Color{255, 240, 180, 255});
            DrawSphereWires({lampBase.x - 0.85F, 3.55F, lampBase.z}, 0.22F, 6, 6, Fade(Color{255, 215, 100, 255}, 0.6F));
            DrawCylinder({lampBase.x - 0.85F, 0.01F, lampBase.z}, 1.1F, 1.1F, 0.01F, 12, Fade(Color{255, 240, 160, 255}, 0.10F));
        }
        else
        {
            // Árvore frondosa multicamada
            const Vector3 treeBase{sidewalkLeftX - 0.3F, 0.0F, segmentZ + 1.0F};
            DrawCylinder(treeBase, 0.20F, 0.26F, 2.0F, 8, Color{115, 78, 42, 255});
            DrawSphere({treeBase.x, 2.5F, treeBase.z}, 1.2F, Color{ 85, 140,  70, 255});
            DrawSphere({treeBase.x, 3.3F, treeBase.z}, 0.9F, Color{ 95, 158,  78, 255});
            DrawSphere({treeBase.x, 4.0F, treeBase.z}, 0.6F, Color{110, 175,  88, 255});
            DrawCylinder({treeBase.x + 0.3F, 0.01F, treeBase.z + 0.3F}, 0.8F, 0.8F, 0.01F, 12, Fade(BLACK, 0.14F));

            // Hidrante vintage dessaturado
            const Vector3 hydrantPos{sidewalkRightX + 0.5F, 0.0F, segmentZ - 2.5F};
            DrawCylinder(hydrantPos, 0.14F, 0.16F, 0.65F, 8, Color{175, 55, 50, 255});
            DrawSphere({hydrantPos.x, 0.65F, hydrantPos.z}, 0.14F, Color{175, 55, 50, 255});
            DrawCube({hydrantPos.x, 0.40F, hydrantPos.z}, 0.40F, 0.10F, 0.10F, Color{150, 40, 38, 255});
            DrawSphere({hydrantPos.x + 0.17F, 0.35F, hydrantPos.z}, 0.05F, Color{200, 200, 195, 255});
            DrawSphere({hydrantPos.x - 0.17F, 0.35F, hydrantPos.z}, 0.05F, Color{200, 200, 195, 255});
        }

        // ==========================================
        // 3. PRÉDIOS URBANOS — FACHADAS RICAS
        // ==========================================
        const float bldgZ     = segmentZ;
        const float bldgDepth = Settings::TRACK_SEGMENT_LENGTH - 0.3F;

        // --- PRÉDIO ESQUERDO ---
        const float bldgLeftX      = -halfTrack - sidewalkWidth - 3.5F;
        const float bldgLeftHeight =  9.0F + static_cast<float>((idx * 4) % 11);
        const Color bldgLeftColor  = buildingColorsLeft[idx % 5];

        DrawCube({bldgLeftX, bldgLeftHeight * 0.5F, bldgZ}, 7.0F, bldgLeftHeight, bldgDepth, bldgLeftColor);
        // Painel frontal levemente mais claro (parede de frente)
        DrawCube({bldgLeftX + 3.48F, bldgLeftHeight * 0.5F, bldgZ}, 0.08F, bldgLeftHeight - 0.5F, bldgDepth - 0.4F,
                 Color{static_cast<unsigned char>(std::min(255, (int)bldgLeftColor.r + 18)),
                       static_cast<unsigned char>(std::min(255, (int)bldgLeftColor.g + 15)),
                       static_cast<unsigned char>(std::min(255, (int)bldgLeftColor.b + 12)), 255});
        DrawCubeWires({bldgLeftX, bldgLeftHeight * 0.5F, bldgZ}, 7.0F, bldgLeftHeight, bldgDepth, Color{35, 32, 28, 200});

        // Janelas com moldura
        const float winFaceX = bldgLeftX + 3.52F;
        for (float wy = 2.0F; wy < bldgLeftHeight - 1.5F; wy += 2.2F)
        {
            for (float wz = bldgZ - 3.5F; wz <= bldgZ + 3.5F; wz += 2.4F)
            {
                DrawCube({winFaceX, wy, wz}, 0.07F, 1.4F, 1.4F, windowFrameColor);
                const bool lit = ((static_cast<int>(wy * 7.0F + wz * 3.0F)) % 3 != 0);
                DrawCube({winFaceX, wy, wz}, 0.055F, 1.1F, 1.1F,
                         lit ? Color{255, 238, 160, 255} : Color{130, 165, 200, 220});
            }
        }
        // Toldo térreo
        if (bldgLeftHeight > 10.0F)
        {
            DrawCube({bldgLeftX + 3.52F, 1.5F, bldgZ}, 0.14F, 0.12F, 2.0F,
                     Color{static_cast<unsigned char>(std::min(255, (int)bldgLeftColor.r - 20)),
                           static_cast<unsigned char>(std::min(255, (int)bldgLeftColor.g - 15)),
                           static_cast<unsigned char>(std::min(255, (int)bldgLeftColor.b - 10)), 255});
        }
        // Friso topo + caixa d'água decorativa
        DrawCube({bldgLeftX, bldgLeftHeight + 0.2F, bldgZ}, 7.4F, 0.4F, bldgDepth + 0.4F, Color{58, 54, 48, 255});
        if (idx % 3 == 0)
        {
            DrawCylinder({bldgLeftX - 1.0F, bldgLeftHeight + 1.0F, bldgZ}, 0.5F, 0.5F, 1.2F, 10, Color{130, 118, 100, 255});
            DrawCylinder({bldgLeftX - 1.0F, bldgLeftHeight + 2.2F, bldgZ}, 0.52F, 0.42F, 0.3F, 10, Color{100, 90, 78, 255});
        }

        // --- PRÉDIO DIREITO ---
        const float bldgRightX      =  halfTrack + sidewalkWidth + 3.5F;
        const float bldgRightHeight =  8.5F + static_cast<float>(((idx + 2) * 5) % 13);
        const Color bldgRightColor  = buildingColorsRight[idx % 5];

        DrawCube({bldgRightX, bldgRightHeight * 0.5F, bldgZ}, 7.0F, bldgRightHeight, bldgDepth, bldgRightColor);
        DrawCube({bldgRightX - 3.48F, bldgRightHeight * 0.5F, bldgZ}, 0.08F, bldgRightHeight - 0.5F, bldgDepth - 0.4F,
                 Color{static_cast<unsigned char>(std::min(255, (int)bldgRightColor.r + 18)),
                       static_cast<unsigned char>(std::min(255, (int)bldgRightColor.g + 15)),
                       static_cast<unsigned char>(std::min(255, (int)bldgRightColor.b + 12)), 255});
        DrawCubeWires({bldgRightX, bldgRightHeight * 0.5F, bldgZ}, 7.0F, bldgRightHeight, bldgDepth, Color{35, 32, 28, 200});

        const float winRightFaceX = bldgRightX - 3.52F;
        for (float wy = 2.0F; wy < bldgRightHeight - 1.5F; wy += 2.2F)
        {
            for (float wz = bldgZ - 3.5F; wz <= bldgZ + 3.5F; wz += 2.4F)
            {
                DrawCube({winRightFaceX, wy, wz}, 0.07F, 1.4F, 1.4F, windowFrameColor);
                const bool lit = ((static_cast<int>(wy * 5.0F + wz * 4.0F)) % 3 == 0);
                DrawCube({winRightFaceX, wy, wz}, 0.055F, 1.1F, 1.1F,
                         lit ? Color{255, 238, 160, 255} : Color{115, 150, 185, 220});
            }
        }
        if (bldgRightHeight > 10.0F)
        {
            DrawCube({bldgRightX - 3.52F, 1.5F, bldgZ}, 0.14F, 0.12F, 2.0F,
                     Color{static_cast<unsigned char>(std::min(255, (int)bldgRightColor.r - 20)),
                           static_cast<unsigned char>(std::min(255, (int)bldgRightColor.g - 15)),
                           static_cast<unsigned char>(std::min(255, (int)bldgRightColor.b - 10)), 255});
        }
        DrawCube({bldgRightX, bldgRightHeight + 0.2F, bldgZ}, 7.4F, 0.4F, bldgDepth + 0.4F, Color{58, 54, 48, 255});

        // Outdoor
        if (idx % 3 == 1)
        {
            const Vector3 signPos{bldgRightX - 0.5F, bldgRightHeight + 1.2F, bldgZ};
            DrawCube(signPos, 2.6F, 1.7F, 4.6F, Color{40, 38, 35, 255});
            const Color signGlow = (idx % 2 == 0) ? Color{130, 190, 100, 255} : Color{90, 145, 195, 255};
            DrawCube({signPos.x - 1.26F, signPos.y, signPos.z}, 0.07F, 1.4F, 4.2F, signGlow);
        }

        // Sacada / varanda em alguns prédios
        if (idx % 4 == 2 && bldgRightHeight > 11.0F)
        {
            const float balconyY = bldgRightHeight * 0.5F;
            DrawCube({bldgRightX - 3.65F, balconyY, bldgZ}, 0.5F, 0.1F, 2.2F, Color{80, 75, 68, 255});
            for (float gz = bldgZ - 0.9F; gz <= bldgZ + 0.9F; gz += 0.5F)
            {
                DrawCube({bldgRightX - 3.78F, balconyY + 0.2F, gz}, 0.06F, 0.4F, 0.06F, Color{100, 95, 88, 255});
            }
        }
    }
}
