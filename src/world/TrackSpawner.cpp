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
    // 1. CÉU, SOL RADIANTE E NUVENS ESTILIZADAS
    // ==========================================

    // Sol estilizado brilhante (amarelo dourado com corona)
    DrawSphere({24.0F, 30.0F, 86.0F}, 5.5F, Color{255, 230, 80, 255});
    DrawSphereWires({24.0F, 30.0F, 86.0F}, 6.5F, 10, 10, Color{255, 245, 140, 160});

    // Nuvens fofas brancas flutuando
    const float cloudOffset = std::fmod(time * 1.4F, 60.0F);
    for (float cz = 15.0F; cz <= 85.0F; cz += 28.0F)
    {
        const float cloudZ = cz - cloudOffset + 10.0F;
        // Nuvem esquerda
        DrawSphere({-19.0F, 24.0F, cloudZ},        3.2F, Color{250, 252, 255, 235});
        DrawSphere({-16.0F, 24.6F, cloudZ},        4.2F, Color{255, 255, 255, 245});
        DrawSphere({-12.8F, 24.0F, cloudZ},        3.0F, Color{250, 252, 255, 235});

        // Nuvem direita
        DrawSphere({ 16.0F, 26.0F, cloudZ + 14.0F}, 3.5F, Color{250, 252, 255, 235});
        DrawSphere({ 19.5F, 26.7F, cloudZ + 14.0F}, 4.4F, Color{255, 255, 255, 245});
        DrawSphere({ 23.2F, 26.0F, cloudZ + 14.0F}, 3.2F, Color{250, 252, 255, 235});
    }

    // Silhuetas distantes da metrópole no horizonte (Z = 96 a 100)
    DrawCube({-32.0F, 12.0F, 96.0F}, 14.0F, 26.0F, 4.0F, Color{120, 165, 210, 255});
    DrawCube({-16.0F, 16.0F, 98.0F}, 12.0F, 34.0F, 4.0F, Color{110, 155, 200, 255});
    DrawCube({  0.0F, 20.0F, 100.0F},16.0F, 42.0F, 4.0F, Color{100, 145, 190, 255});
    DrawCube({ 18.0F, 14.0F, 97.0F}, 14.0F, 30.0F, 4.0F, Color{115, 160, 205, 255});
    DrawCube({ 34.0F, 11.0F, 95.0F}, 12.0F, 24.0F, 4.0F, Color{120, 165, 210, 255});

    // ==========================================
    // 2. SEGMENTOS DA PISTA E CIDADE
    // ==========================================
    const float halfTrack = Settings::TRACK_WIDTH * 0.5F; // 4.1F
    constexpr float sidewalkWidth = 2.6F;
    const float sidewalkLeftX  = -halfTrack - (sidewalkWidth * 0.5F);
    const float sidewalkRightX =  halfTrack + (sidewalkWidth * 0.5F);

    // Paleta de prédios pastel/harmonizada (vibrante e amigável, sem cores primárias agressivas)
    // Esquerda: Terracota suave, Azul ardósia, Areia quente, Mostarda suave, Rosa antigo
    const Color buildingColorsLeft[] = {
        Color{225, 138, 108, 255},  // Terracota suave
        Color{115, 155, 195, 255},  // Azul ardósia
        Color{230, 198, 142, 255},  // Areia quente
        Color{155, 188, 138, 255},  // Verde sálvia
        Color{210, 130, 145, 255}   // Rosa antigo
    };

    // Direita: Menta fresca, Coral suave, Jeans suave, Manteiga quente, Lavanda suave
    const Color buildingColorsRight[] = {
        Color{135, 192, 155, 255},  // Menta fresca
        Color{230, 148, 118, 255},  // Coral suave
        Color{125, 162, 202, 255},  // Jeans suave
        Color{238, 208, 138, 255},  // Manteiga quente
        Color{178, 148, 192, 255}   // Lavanda suave
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
            Color{44, 48, 56, 255});

        // --- LINHAS TRACEJADAS BRANCAS DIVISÓRIAS DAS 3 RAIAS ---
        for (const float laneMarkerX : {-Settings::LANE_WIDTH * 0.5F, Settings::LANE_WIDTH * 0.5F})
        {
            for (int k = -1; k <= 1; ++k)
            {
                const float dashZ = segmentZ + static_cast<float>(k) * 3.8F;
                DrawCube({laneMarkerX, 0.02F, dashZ}, 0.10F, 0.02F, 2.2F, RAYWHITE);
            }
        }

        // --- GUIAS / MEIO-FIO ZEBRADO (Amarelo Ouro e Carvão) ---
        for (int k = -2; k <= 2; ++k)
        {
            const float curbZ = segmentZ + static_cast<float>(k) * 2.4F;
            const Color curbColor = (std::abs(k + static_cast<int>(idx)) % 2 == 0)
                ? Color{245, 195, 35, 255}
                : Color{35, 35, 40, 255};
            DrawCube({-halfTrack, 0.06F, curbZ}, 0.18F, 0.12F, 2.4F, curbColor);
            DrawCube({ halfTrack, 0.06F, curbZ}, 0.18F, 0.12F, 2.4F, curbColor);
        }

        // --- CALÇADAS LATERAIS DE CONCRETO CLARO ---
        DrawCube({sidewalkLeftX,  -0.06F, segmentZ}, sidewalkWidth, 0.24F, Settings::TRACK_SEGMENT_LENGTH, Color{215, 218, 222, 255});
        DrawCube({sidewalkRightX, -0.06F, segmentZ}, sidewalkWidth, 0.24F, Settings::TRACK_SEGMENT_LENGTH, Color{215, 218, 222, 255});

        // Ranhuras/juntas das placas de concreto da calçada
        for (int k = -1; k <= 1; ++k)
        {
            const float seamZ = segmentZ + static_cast<float>(k) * 4.0F;
            DrawCube({sidewalkLeftX,  0.07F, seamZ}, sidewalkWidth, 0.02F, 0.06F, Color{165, 168, 175, 255});
            DrawCube({sidewalkRightX, 0.07F, seamZ}, sidewalkWidth, 0.02F, 0.06F, Color{165, 168, 175, 255});
        }

        // --- ELEMENTOS URBANOS NAS CALÇADAS (Alternância a cada segmento) ---
        const int propType = static_cast<int>(idx) % 4;

        if (propType == 0)
        {
            // Floreira charmosa na calçada esquerda
            const Vector3 planterPos{sidewalkLeftX, 0.16F, segmentZ - 2.0F};
            DrawCube(planterPos, 0.70F, 0.24F, 1.4F, Color{140, 142, 148, 255});
            DrawCube({planterPos.x, planterPos.y + 0.10F, planterPos.z}, 0.60F, 0.10F, 1.3F, Color{75, 48, 25, 255});
            // Arbustos verdes
            DrawSphere({planterPos.x, planterPos.y + 0.25F, planterPos.z - 0.35F}, 0.28F, Color{60, 180, 85, 255});
            DrawSphere({planterPos.x, planterPos.y + 0.28F, planterPos.z},          0.30F, Color{45, 165, 70, 255});
            DrawSphere({planterPos.x, planterPos.y + 0.25F, planterPos.z + 0.35F}, 0.28F, Color{60, 180, 85, 255});
            // Florzinhas
            DrawSphere({planterPos.x, planterPos.y + 0.40F, planterPos.z - 0.2F}, 0.08F, RED);
            DrawSphere({planterPos.x, planterPos.y + 0.42F, planterPos.z + 0.1F}, 0.08F, YELLOW);
            DrawSphere({planterPos.x, planterPos.y + 0.40F, planterPos.z + 0.3F}, 0.08F, Color{255, 105, 180, 255});

            // Poste de iluminação na calçada direita
            const Vector3 lampBase{sidewalkRightX + 0.4F, 0.0F, segmentZ + 2.0F};
            DrawCylinder(lampBase, 0.09F, 0.12F, 3.6F, 8, Color{50, 55, 62, 255});
            DrawCube({lampBase.x - 0.45F, 3.6F, lampBase.z}, 0.9F, 0.09F, 0.09F, Color{50, 55, 62, 255});
            DrawSphere({lampBase.x - 0.85F, 3.55F, lampBase.z}, 0.20F, Color{255, 248, 190, 255});
            DrawSphereWires({lampBase.x - 0.85F, 3.55F, lampBase.z}, 0.22F, 6, 6, Fade(GOLD, 0.5F));
        }
        else if (propType == 1)
        {
            // Árvore frondosa na calçada esquerda com sombra
            const Vector3 treeBase{sidewalkLeftX - 0.3F, 0.0F, segmentZ + 1.0F};
            DrawCylinder(treeBase, 0.20F, 0.26F, 2.0F, 8, Color{115, 75, 42, 255});
            DrawSphere({treeBase.x, 2.5F, treeBase.z}, 1.2F, Color{55, 175, 80, 255});
            DrawSphere({treeBase.x, 3.3F, treeBase.z}, 0.9F, Color{70, 195, 95, 255});
            DrawSphere({treeBase.x, 4.0F, treeBase.z}, 0.6F, Color{90, 215, 115, 255});
            // Sombra suave da árvore no chão
            DrawCylinder({treeBase.x + 0.2F, 0.065F, treeBase.z}, 0.9F, 0.9F, 0.01F, 12, Fade(BLACK, 0.18F));

            // Hidrante vermelho clássico na calçada direita
            const Vector3 hydrantPos{sidewalkRightX + 0.5F, 0.0F, segmentZ - 2.5F};
            DrawCylinder(hydrantPos, 0.14F, 0.16F, 0.65F, 8, Color{225, 45, 40, 255});
            DrawSphere({hydrantPos.x, 0.65F, hydrantPos.z}, 0.14F, Color{225, 45, 40, 255});
            DrawCube({hydrantPos.x, 0.40F, hydrantPos.z}, 0.40F, 0.10F, 0.10F, Color{180, 30, 25, 255});
        }
        else if (propType == 2)
        {
            // Banco de praça de madeira na calçada esquerda
            const Vector3 benchPos{sidewalkLeftX, 0.20F, segmentZ};
            DrawCube(benchPos, 0.50F, 0.08F, 1.30F, Color{160, 95, 45, 255}); // Assento
            DrawCube({benchPos.x - 0.20F, benchPos.y + 0.25F, benchPos.z}, 0.08F, 0.45F, 1.30F, Color{160, 95, 45, 255}); // Encosto
            DrawCube({benchPos.x, benchPos.y - 0.10F, benchPos.z - 0.55F}, 0.45F, 0.20F, 0.08F, Color{45, 48, 52, 255}); // Perna 1
            DrawCube({benchPos.x, benchPos.y - 0.10F, benchPos.z + 0.55F}, 0.45F, 0.20F, 0.08F, Color{45, 48, 52, 255}); // Perna 2

            // Poste de iluminação na calçada direita
            const Vector3 lampBase{sidewalkRightX + 0.4F, 0.0F, segmentZ - 1.5F};
            DrawCylinder(lampBase, 0.09F, 0.12F, 3.6F, 8, Color{50, 55, 62, 255});
            DrawCube({lampBase.x - 0.45F, 3.6F, lampBase.z}, 0.9F, 0.09F, 0.09F, Color{50, 55, 62, 255});
            DrawSphere({lampBase.x - 0.85F, 3.55F, lampBase.z}, 0.20F, Color{255, 248, 190, 255});
            DrawSphereWires({lampBase.x - 0.85F, 3.55F, lampBase.z}, 0.22F, 6, 6, Fade(GOLD, 0.5F));
        }
        else
        {
            // Lixeira urbana metálica na calçada esquerda
            const Vector3 binPos{sidewalkLeftX - 0.2F, 0.0F, segmentZ - 1.0F};
            DrawCylinder(binPos, 0.22F, 0.20F, 0.70F, 10, Color{70, 78, 88, 255});
            DrawCylinder({binPos.x, 0.70F, binPos.z}, 0.24F, 0.22F, 0.08F, 10, Color{45, 52, 60, 255});

            // Árvore na calçada direita com sombra
            const Vector3 treeBase{sidewalkRightX + 0.4F, 0.0F, segmentZ + 1.5F};
            DrawCylinder(treeBase, 0.20F, 0.26F, 2.0F, 8, Color{115, 75, 42, 255});
            DrawSphere({treeBase.x, 2.5F, treeBase.z}, 1.2F, Color{55, 175, 80, 255});
            DrawSphere({treeBase.x, 3.3F, treeBase.z}, 0.9F, Color{70, 195, 95, 255});
            DrawSphere({treeBase.x, 4.0F, treeBase.z}, 0.6F, Color{90, 215, 115, 255});
            DrawCylinder({treeBase.x - 0.2F, 0.065F, treeBase.z}, 0.9F, 0.9F, 0.01F, 12, Fade(BLACK, 0.18F));
        }

        // ==========================================
        // 3. PRÉDIOS URBANOS (Arquitetura Rica)
        // ==========================================
        const float bldgZ = segmentZ;
        const float bldgDepth = Settings::TRACK_SEGMENT_LENGTH - 0.4F;

        // --- PRÉDIO ESQUERDO ---
        const float bldgLeftX = -halfTrack - sidewalkWidth - 3.5F;
        const float bldgLeftHeight = 9.0F + static_cast<float>((idx * 4) % 11);
        const Color bldgLeftColor = buildingColorsLeft[idx % 5];

        DrawCube({bldgLeftX, bldgLeftHeight * 0.5F, bldgZ}, 7.0F, bldgLeftHeight, bldgDepth, bldgLeftColor);
        DrawCubeWires({bldgLeftX, bldgLeftHeight * 0.5F, bldgZ}, 7.0F, bldgLeftHeight, bldgDepth, Fade(BLACK, 0.20F));

        // Janelas com moldura (offset seguro de +3.54F para ZERO z-fighting)
        const float winFaceX = bldgLeftX + 3.54F;
        int rowIndex = 0;
        for (float wy = 2.2F; wy < bldgLeftHeight - 1.5F; wy += 2.2F, ++rowIndex)
        {
            for (int columnIndex = 0; columnIndex < 4; ++columnIndex)
            {
                const float wz = bldgZ - 3.6F + static_cast<float>(columnIndex) * 2.4F;
                // Moldura escura
                DrawCube({winFaceX, wy, wz}, 0.04F, 1.25F, 1.25F, Color{50, 48, 52, 255});
                const bool lit = (static_cast<int>(idx) + rowIndex * 3 + columnIndex * 5) % 3 != 0;
                const Color winColor = lit ? Color{255, 245, 170, 255} : Color{165, 205, 235, 255};
                DrawCube({winFaceX + 0.02F, wy, wz}, 0.04F, 1.05F, 1.05F, winColor);
            }
        }

        // Toldo charmoso no térreo
        if (bldgLeftHeight > 10.0F)
        {
            const Vector3 awningPos{bldgLeftX + 3.85F, 2.1F, bldgZ};
            DrawCube(awningPos, 0.75F, 0.12F, 2.8F, (idx % 2 == 0) ? Color{220, 75, 60, 255} : Color{50, 140, 220, 255});
        }

        // Platibanda / Friso no teto
        DrawCube({bldgLeftX, bldgLeftHeight + 0.2F, bldgZ}, 7.3F, 0.4F, bldgDepth + 0.3F, Color{60, 58, 65, 255});

        // Caixa d'água no teto
        if (idx % 3 == 0)
        {
            DrawCylinder({bldgLeftX - 0.8F, bldgLeftHeight + 0.4F, bldgZ}, 0.65F, 0.65F, 1.2F, 10, Color{125, 130, 138, 255});
        }

        // --- PRÉDIO DIREITO ---
        const float bldgRightX = halfTrack + sidewalkWidth + 3.5F;
        const float bldgRightHeight = 8.5F + static_cast<float>(((idx + 2) * 5) % 13);
        const Color bldgRightColor = buildingColorsRight[idx % 5];

        DrawCube({bldgRightX, bldgRightHeight * 0.5F, bldgZ}, 7.0F, bldgRightHeight, bldgDepth, bldgRightColor);
        DrawCubeWires({bldgRightX, bldgRightHeight * 0.5F, bldgZ}, 7.0F, bldgRightHeight, bldgDepth, Fade(BLACK, 0.20F));

        // Janelas direitas (offset seguro de -3.54F)
        const float winRightFaceX = bldgRightX - 3.54F;
        rowIndex = 0;
        for (float wy = 2.2F; wy < bldgRightHeight - 1.5F; wy += 2.2F, ++rowIndex)
        {
            for (int columnIndex = 0; columnIndex < 4; ++columnIndex)
            {
                const float wz = bldgZ - 3.6F + static_cast<float>(columnIndex) * 2.4F;
                DrawCube({winRightFaceX, wy, wz}, 0.04F, 1.25F, 1.25F, Color{50, 48, 52, 255});
                const bool lit = (static_cast<int>(idx) + rowIndex * 5 + columnIndex * 3) % 3 == 0;
                const Color winColor = lit ? Color{255, 245, 170, 255} : Color{165, 205, 235, 255};
                DrawCube({winRightFaceX - 0.02F, wy, wz}, 0.04F, 1.05F, 1.05F, winColor);
            }
        }

        // Toldo charmoso no térreo direito
        if (bldgRightHeight > 10.0F)
        {
            const Vector3 awningRightPos{bldgRightX - 3.85F, 2.1F, bldgZ};
            DrawCube(awningRightPos, 0.75F, 0.12F, 2.8F, (idx % 2 == 0) ? Color{235, 175, 45, 255} : Color{45, 185, 115, 255});
        }

        // Platibanda no teto do prédio direito
        DrawCube({bldgRightX, bldgRightHeight + 0.2F, bldgZ}, 7.3F, 0.4F, bldgDepth + 0.3F, Color{60, 58, 65, 255});

        // Sacada no meio do prédio
        if (idx % 3 == 1 && bldgRightHeight > 11.0F)
        {
            const float balconyY = 5.0F;
            DrawCube({bldgRightX - 3.8F, balconyY, bldgZ}, 0.7F, 0.12F, 2.2F, Color{85, 88, 95, 255});
            DrawCube({bldgRightX - 4.1F, balconyY + 0.25F, bldgZ}, 0.08F, 0.40F, 2.2F, Color{65, 68, 75, 255});
        }

        // Outdoor publicitário em alguns prédios
        if (idx % 3 == 2)
        {
            const Vector3 signPos{bldgRightX - 0.5F, bldgRightHeight + 1.2F, bldgZ};
            DrawCube(signPos, 2.5F, 1.6F, 4.5F, Color{45, 48, 54, 255});
            DrawCube({signPos.x - 1.28F, signPos.y, signPos.z}, 0.06F, 1.4F, 4.2F, (idx % 2 == 0) ? LIME : SKYBLUE);
        }
    }
}
