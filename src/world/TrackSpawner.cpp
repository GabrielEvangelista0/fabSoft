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
    // 1. CÉU, SOL E NUVENS FLUTUANTES
    // ==========================================
    // Sol radiante com corona
    DrawSphere({22.0F, 28.0F, 82.0F}, 6.5F, Color{255, 215, 0, 255});
    DrawSphereWires({22.0F, 28.0F, 82.0F}, 7.2F, 12, 12, Color{255, 235, 130, 180});

    // Nuvens fofas flutuando suavemente no céu
    const float cloudOffset = std::fmod(time * 1.5F, 50.0F);
    for (float cz = 20.0F; cz <= 80.0F; cz += 25.0F)
    {
        const float cloudZ = cz - cloudOffset + 15.0F;
        DrawSphere({-18.0F, 24.0F, cloudZ}, 3.5F, Fade(RAYWHITE, 0.85F));
        DrawSphere({-15.0F, 24.5F, cloudZ}, 4.2F, Fade(RAYWHITE, 0.90F));
        DrawSphere({-12.0F, 24.0F, cloudZ}, 3.2F, Fade(RAYWHITE, 0.85F));

        DrawSphere({15.0F, 26.0F, cloudZ + 10.0F}, 3.8F, Fade(RAYWHITE, 0.85F));
        DrawSphere({18.5F, 26.5F, cloudZ + 10.0F}, 4.5F, Fade(RAYWHITE, 0.90F));
        DrawSphere({22.0F, 26.0F, cloudZ + 10.0F}, 3.4F, Fade(RAYWHITE, 0.85F));
    }

    // Silhuetas distantes da metrópole no horizonte (Z = 95)
    DrawCube({-32.0F, 12.0F, 96.0F}, 14.0F, 26.0F, 6.0F, Color{85, 105, 130, 255});
    DrawCube({-16.0F, 16.0F, 98.0F}, 12.0F, 34.0F, 6.0F, Color{70, 90, 115, 255});
    DrawCube({0.0F, 20.0F, 100.0F}, 16.0F, 42.0F, 6.0F, Color{60, 80, 105, 255});
    DrawCube({18.0F, 14.0F, 97.0F}, 14.0F, 30.0F, 6.0F, Color{75, 95, 120, 255});
    DrawCube({34.0F, 11.0F, 95.0F}, 12.0F, 24.0F, 6.0F, Color{85, 105, 130, 255});

    // ==========================================
    // 2. SEGMENTOS DA PISTA E CIDADE
    // ==========================================
    const float halfTrack = Settings::TRACK_WIDTH * 0.5F; // 4.1F

    // Cores de prédios urbanos
    const Color buildingColorsLeft[] = {
        Color{192, 57, 43, 255},   // Tijolo clássico
        Color{41, 128, 185, 255},  // Azul metropolitano moderno
        Color{211, 84, 0, 255},    // Laranja queimado
        Color{52, 73, 94, 255},    // Ardósia elegante
        Color{142, 68, 173, 255}   // Púrpura urbano
    };

    const Color buildingColorsRight[] = {
        Color{39, 174, 96, 255},   // Verde esmeralda
        Color{230, 126, 34, 255},  // Âmbar
        Color{44, 62, 80, 255},    // Marinho profundo
        Color{127, 140, 141, 255}, // Prata concreto
        Color{180, 95, 60, 255}    // Terracota
    };

    for (size_t idx = 0; idx < track.segmentZ.size(); ++idx)
    {
        const float segmentZ = track.segmentZ[idx];

        // --- ASFALTO PRINCIPAL DA PISTA ---
        DrawCube(
            {0.0F, -0.16F, segmentZ},
            Settings::TRACK_WIDTH,
            0.32F,
            Settings::TRACK_SEGMENT_LENGTH,
            Color{36, 40, 48, 255});

        // --- LINHAS TRACEJADAS BRANCAS DIVISÓRIAS DAS 3 RAIAS ---
        for (const float laneMarkerX : {-Settings::LANE_WIDTH * 0.5F, Settings::LANE_WIDTH * 0.5F})
        {
            for (int k = -1; k <= 1; ++k)
            {
                const float dashZ = segmentZ + static_cast<float>(k) * 3.8F;
                DrawCube({laneMarkerX, 0.02F, dashZ}, 0.10F, 0.02F, 2.2F, RAYWHITE);
            }
        }

        // --- GUIAS / MEIO-FIO ZEBRADO (Amarelo e Preto) ---
        for (int k = -2; k <= 2; ++k)
        {
            const float curbZ = segmentZ + static_cast<float>(k) * 2.4F;
            const Color curbColor = (std::abs(k + static_cast<int>(idx)) % 2 == 0) ? GOLD : Color{25, 25, 25, 255};
            DrawCube({-halfTrack, 0.06F, curbZ}, 0.18F, 0.12F, 2.4F, curbColor);
            DrawCube({halfTrack, 0.06F, curbZ}, 0.18F, 0.12F, 2.4F, curbColor);
        }

        // --- CALÇADAS LATERAIS DE CONCRETO ---
        constexpr float sidewalkWidth = 2.6F;
        const float sidewalkLeftX = -halfTrack - (sidewalkWidth * 0.5F);
        const float sidewalkRightX = halfTrack + (sidewalkWidth * 0.5F);

        DrawCube({sidewalkLeftX, -0.06F, segmentZ}, sidewalkWidth, 0.24F, Settings::TRACK_SEGMENT_LENGTH, Color{195, 200, 205, 255});
        DrawCube({sidewalkRightX, -0.06F, segmentZ}, sidewalkWidth, 0.24F, Settings::TRACK_SEGMENT_LENGTH, Color{195, 200, 205, 255});

        // Ranhuras/juntas das placas de concreto da calçada
        for (int k = -1; k <= 1; ++k)
        {
            const float seamZ = segmentZ + static_cast<float>(k) * 4.0F;
            DrawCube({sidewalkLeftX, 0.07F, seamZ}, sidewalkWidth, 0.02F, 0.06F, Color{130, 135, 140, 255});
            DrawCube({sidewalkRightX, 0.07F, seamZ}, sidewalkWidth, 0.02F, 0.06F, Color{130, 135, 140, 255});
        }

        // --- ELEMENTOS URBANOS NAS CALÇADAS (Postes, Árvores e Floreiras) ---
        if (idx % 2 == 0)
        {
            // Floreira charmosa na calçada esquerda
            const Vector3 planterPos{sidewalkLeftX, 0.16F, segmentZ - 2.0F};
            DrawCube(planterPos, 0.70F, 0.24F, 1.4F, Color{110, 115, 120, 255});
            DrawCube({planterPos.x, planterPos.y + 0.10F, planterPos.z}, 0.60F, 0.10F, 1.3F, Color{60, 40, 20, 255}); // Terra
            // Arbustos floridos
            DrawSphere({planterPos.x, planterPos.y + 0.25F, planterPos.z - 0.35F}, 0.28F, Color{46, 204, 113, 255});
            DrawSphere({planterPos.x, planterPos.y + 0.28F, planterPos.z}, 0.30F, Color{39, 174, 96, 255});
            DrawSphere({planterPos.x, planterPos.y + 0.25F, planterPos.z + 0.35F}, 0.28F, Color{46, 204, 113, 255});
            // Florzinhas coloridas
            DrawSphere({planterPos.x, planterPos.y + 0.40F, planterPos.z - 0.2F}, 0.08F, RED);
            DrawSphere({planterPos.x, planterPos.y + 0.42F, planterPos.z + 0.1F}, 0.08F, YELLOW);
            DrawSphere({planterPos.x, planterPos.y + 0.40F, planterPos.z + 0.3F}, 0.08F, Color{255, 105, 180, 255});

            // Poste de Iluminação na calçada direita
            const Vector3 lampBase{sidewalkRightX + 0.4F, 0.0F, segmentZ + 2.0F};
            DrawCylinder(lampBase, 0.09F, 0.12F, 3.6F, 8, Color{45, 52, 54, 255});
            // Haste horizontal curva
            DrawCube({lampBase.x - 0.45F, 3.6F, lampBase.z}, 0.9F, 0.09F, 0.09F, Color{45, 52, 54, 255});
            // Cúpula da luminária
            DrawSphere({lampBase.x - 0.85F, 3.55F, lampBase.z}, 0.20F, Color{255, 255, 190, 255});
            DrawSphereWires({lampBase.x - 0.85F, 3.55F, lampBase.z}, 0.22F, 6, 6, Fade(GOLD, 0.5F));
        }
        else
        {
            // Árvore frondosa na calçada esquerda
            const Vector3 treeBase{sidewalkLeftX - 0.3F, 0.0F, segmentZ + 1.0F};
            DrawCylinder(treeBase, 0.20F, 0.26F, 2.0F, 8, Color{110, 68, 35, 255});
            DrawSphere({treeBase.x, 2.5F, treeBase.z}, 1.2F, Color{39, 174, 96, 255});
            DrawSphere({treeBase.x, 3.3F, treeBase.z}, 0.9F, Color{46, 204, 113, 255});
            DrawSphere({treeBase.x, 4.0F, treeBase.z}, 0.6F, Color{88, 214, 141, 255});

            // Hidrante clássico vermelho na calçada direita
            const Vector3 hydrantPos{sidewalkRightX + 0.5F, 0.0F, segmentZ - 2.5F};
            DrawCylinder(hydrantPos, 0.14F, 0.16F, 0.65F, 8, RED);
            DrawSphere({hydrantPos.x, 0.65F, hydrantPos.z}, 0.14F, RED);
            DrawCube({hydrantPos.x, 0.40F, hydrantPos.z}, 0.40F, 0.10F, 0.10F, Color{200, 20, 20, 255});
        }

        // ==========================================
        // 3. PRÉDIOS URBANOS (Skyline Subway Surfers)
        // ==========================================
        const float bldgZ = segmentZ;
        const float bldgDepth = Settings::TRACK_SEGMENT_LENGTH - 0.3F;

        // --- PRÉDIO ESQUERDO ---
        const float bldgLeftX = -halfTrack - sidewalkWidth - 3.5F;
        const float bldgLeftHeight = 9.0F + static_cast<float>((idx * 4) % 11);
        const Color bldgLeftColor = buildingColorsLeft[idx % 5];

        DrawCube({bldgLeftX, bldgLeftHeight * 0.5F, bldgZ}, 7.0F, bldgLeftHeight, bldgDepth, bldgLeftColor);
        DrawCubeWires({bldgLeftX, bldgLeftHeight * 0.5F, bldgZ}, 7.0F, bldgLeftHeight, bldgDepth, Color{30, 35, 45, 255});

        // Janelas iluminadas da fachada esquerda
        const float winFaceX = bldgLeftX + 3.52F;
        for (float wy = 2.0F; wy < bldgLeftHeight - 1.5F; wy += 2.2F)
        {
            for (float wz = bldgZ - 3.8F; wz <= bldgZ + 3.8F; wz += 2.5F)
            {
                // Algumas janelas acesas em amarelo, outras refletindo azul
                const bool lit = ((static_cast<int>(wy * 7.0F + wz * 3.0F)) % 3 != 0);
                const Color winColor = lit ? Color{255, 240, 150, 255} : Color{100, 140, 180, 255};
                DrawCube({winFaceX, wy, wz}, 0.06F, 1.2F, 1.2F, winColor);
            }
        }

        // Friso / Platibanda no teto do prédio
        DrawCube({bldgLeftX, bldgLeftHeight + 0.2F, bldgZ}, 7.4F, 0.4F, bldgDepth + 0.4F, Color{50, 55, 65, 255});

        // --- PRÉDIO DIREITO ---
        const float bldgRightX = halfTrack + sidewalkWidth + 3.5F;
        const float bldgRightHeight = 8.5F + static_cast<float>(((idx + 2) * 5) % 13);
        const Color bldgRightColor = buildingColorsRight[idx % 5];

        DrawCube({bldgRightX, bldgRightHeight * 0.5F, bldgZ}, 7.0F, bldgRightHeight, bldgDepth, bldgRightColor);
        DrawCubeWires({bldgRightX, bldgRightHeight * 0.5F, bldgZ}, 7.0F, bldgRightHeight, bldgDepth, Color{30, 35, 45, 255});

        // Janelas iluminadas da fachada direita
        const float winRightFaceX = bldgRightX - 3.52F;
        for (float wy = 2.0F; wy < bldgRightHeight - 1.5F; wy += 2.2F)
        {
            for (float wz = bldgZ - 3.8F; wz <= bldgZ + 3.8F; wz += 2.5F)
            {
                const bool lit = ((static_cast<int>(wy * 5.0F + wz * 4.0F)) % 3 == 0);
                const Color winColor = lit ? Color{255, 240, 150, 255} : Color{90, 130, 170, 255};
                DrawCube({winRightFaceX, wy, wz}, 0.06F, 1.2F, 1.2F, winColor);
            }
        }

        // Friso no teto do prédio direito
        DrawCube({bldgRightX, bldgRightHeight + 0.2F, bldgZ}, 7.4F, 0.4F, bldgDepth + 0.4F, Color{50, 55, 65, 255});

        // Outdoor publicitário em alguns prédios
        if (idx % 3 == 1)
        {
            const Vector3 signPos{bldgRightX - 0.5F, bldgRightHeight + 1.2F, bldgZ};
            DrawCube(signPos, 2.5F, 1.6F, 4.5F, Color{45, 52, 54, 255});
            DrawCube({signPos.x - 1.26F, signPos.y, signPos.z}, 0.06F, 1.4F, 4.2F, (idx % 2 == 0) ? LIME : SKYBLUE);
        }
    }
}
