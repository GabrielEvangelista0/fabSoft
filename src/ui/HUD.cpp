#include "HUD.h"

#include <algorithm>
#include <cmath>

#include "core/Settings.h"

namespace
{
Color GetWeightBarColor(float weight)
{
    if (weight <= 40.0F)
    {
        return Color{40, 200, 60, 255};   // Verde vibrante
    }
    if (weight <= 70.0F)
    {
        return Color{245, 180, 20, 255};  // Amarelo/Dourado alerta
    }
    return Color{230, 40, 40, 255};       // Vermelho perigo
}

void DrawCenteredText(const char* text, int y, int fontSize, Color color)
{
    const int width = MeasureText(text, fontSize);
    DrawText(text, (Settings::WINDOW_WIDTH - width) / 2, y, fontSize, color);
}
}

void DrawHUD(const Player& player, const HUDStats& stats)
{
    // Painel superior semitransparente moderno
    DrawRectangle(0, 0, Settings::WINDOW_WIDTH, 120, Fade(Color{20, 25, 35, 255}, 0.88F));
    DrawRectangle(0, 118, Settings::WINDOW_WIDTH, 2, Color{60, 80, 110, 255});

    // 1. Bloco Esquerdo: Pontuação e Distância
    DrawText("FUJA DAS CALORIAS", 25, 14, 22, SKYBLUE);
    DrawText(TextFormat("PONTOS: %05d", stats.score), 25, 46, 24, RAYWHITE);
    DrawText(TextFormat("DISTANCIA: %d m", static_cast<int>(stats.distance)), 25, 78, 18, LIGHTGRAY);
    DrawText(TextFormat("RECORDE: %d", stats.highScore), 185, 78, 18, GOLD);

    // 2. Bloco Central: Barra de Peso/Saúde
    constexpr int barX = 390;
    constexpr int barY = 50;
    constexpr int barWidth = 380;
    constexpr int barHeight = 26;

    const float weight = player.GetWeight();
    const char* statusLabel = (weight <= 40.0F) ? "SAUDAVEL (FIT)"
        : (weight <= 70.0F) ? "SOBREPESO (LENTO)" : "OBESIDADE GRAVE (FATAL!)";
    const Color statusColor = GetWeightBarColor(weight);

    DrawText("INDICADOR DE PESO / SAUDE:", barX, 22, 17, RAYWHITE);
    DrawText(statusLabel, barX + 245, 22, 17, statusColor);

    // Fundo da barra
    DrawRectangle(barX, barY, barWidth, barHeight, Color{40, 45, 55, 255});
    const float progress = std::clamp(weight / Settings::MAX_WEIGHT, 0.0F, 1.0F);
    DrawRectangle(barX, barY, static_cast<int>(barWidth * progress), barHeight, statusColor);
    DrawRectangleLines(barX, barY, barWidth, barHeight, RAYWHITE);

    // Texto de peso em kg
    DrawText(TextFormat("%.1f / %.0f kg", weight, Settings::MAX_WEIGHT), barX + barWidth + 14, barY + 3, 19, RAYWHITE);

    // 3. Bloco Direito: Controles Rápidos e Áudio
    constexpr int rightX = 890;
    DrawText("CONTROLES RAPIDOS:", rightX, 16, 16, SKYBLUE);
    DrawText("A/D ou Setas: Trocar Raia", rightX, 38, 15, RAYWHITE);
    DrawText("ESPACO/CIMA: Pular | S/BAIXO: Agachar", rightX, 58, 15, RAYWHITE);
    DrawText(TextFormat("P: Pausar | M: Som [%s]", stats.isMuted ? "MUDO" : "LIGADO"), rightX, 80, 15, stats.isMuted ? ORANGE : LIME);

    // Flash de dano na tela
    if (stats.collisionFlash > 0.0F)
    {
        DrawRectangle(0, 0, Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT, Fade(RED, stats.collisionFlash * 0.65F));
    }
}

void DrawMenu(int highScore, bool isMuted)
{
    // Fundo degradê escuro
    DrawRectangleGradientV(0, 0, Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT, Color{15, 25, 45, 255}, Color{5, 10, 20, 255});

    // Título Principal com Sombra
    DrawCenteredText("FUJA DAS CALORIAS!", 70 + 3, 62, BLACK);
    DrawCenteredText("FUJA DAS CALORIAS!", 70, 62, Color{255, 215, 0, 255});
    DrawCenteredText("Endless Runner 3D de Nutricao e Sobrevivencia", 140, 22, SKYBLUE);

    // Card de Instruções e Regras
    constexpr int boxX = 140;
    constexpr int boxY = 185;
    constexpr int boxW = 1000;
    constexpr int boxH = 345;
    DrawRectangle(boxX, boxY, boxW, boxH, Fade(Color{25, 35, 55, 255}, 0.90F));
    DrawRectangleLines(boxX, boxY, boxW, boxH, Color{70, 130, 180, 255});

    DrawText("COMO JOGAR & REGRAS DO JOGO:", boxX + 30, boxY + 22, 22, GOLD);

    DrawText("[+] COMIDAS BOAS (Maca, Cenoura, Carne Grelhada):", boxX + 30, boxY + 60, 19, LIME);
    DrawText("    Reduzem seu peso corporal e concedem pontos de bonificacao!", boxX + 30, boxY + 86, 17, RAYWHITE);

    DrawText("[-] COMIDAS RUINS (Hamburguer, Batata Frita, Refri, Donut):", boxX + 30, boxY + 120, 19, RED);
    DrawText("    Aumentam seu peso! Ao atingir 100 kg, ocorre colapso por obesidade (Game Over).", boxX + 30, boxY + 146, 17, RAYWHITE);

    DrawText("[*] OBSTACULOS DINAMICOS NAS RAIAS:", boxX + 30, boxY + 180, 19, YELLOW);
    DrawText("    - Obstaculo Baixo no chao: PULE com [ESPACO] ou desvie com [A/D].", boxX + 30, boxY + 206, 17, RAYWHITE);
    DrawText("    - Obstaculo Alto no ar: AGACHE com [S / BAIXO] para passar por baixo!", boxX + 30, boxY + 230, 17, RAYWHITE);

    DrawText(TextFormat("MELHOR RECORDE ATUAL: %d PONTOS", highScore), boxX + 30, boxY + 285, 24, GOLD);
    DrawText(TextFormat("[M] Som: %s", isMuted ? "MUTADO" : "ATIVADO"), boxX + boxW - 220, boxY + 288, 18, isMuted ? ORANGE : LIME);

    // Chamada para Iniciar (Piscante)
    const float blink = std::sin(static_cast<float>(GetTime()) * 5.0F);
    const Color promptColor = (blink > 0.0F) ? LIME : RAYWHITE;
    DrawCenteredText(">> PRESSIONE [ENTER] PARA INICIAR A CORRIDA <<", 560, 26, promptColor);
    DrawCenteredText("Pressione [ESC] para sair da aplicacao", 605, 18, LIGHTGRAY);
}

void DrawGameOver(const HUDStats& stats)
{
    // Fundo escuro com vinheta avermelhada
    DrawRectangleGradientV(0, 0, Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT, Color{35, 10, 10, 255}, Color{10, 5, 5, 255});

    DrawCenteredText("FIM DE JOGO!", 60 + 3, 64, BLACK);
    DrawCenteredText("FIM DE JOGO!", 60, 64, Color{235, 45, 45, 255});

    DrawCenteredText("Causa da Morte: Obesidade extrema e falencia cardiaca por excesso de calorias!", 135, 20, RAYWHITE);

    if (stats.isNewRecord)
    {
        DrawCenteredText("*** NOVO RECORDE ALCANCADO! ***", 175, 26, GOLD);
    }

    // Painel de Estatísticas da Partida
    constexpr int boxX = 260;
    constexpr int boxY = 220;
    constexpr int boxW = 760;
    constexpr int boxH = 280;
    DrawRectangle(boxX, boxY, boxW, boxH, Fade(Color{30, 20, 25, 255}, 0.92F));
    DrawRectangleLines(boxX, boxY, boxW, boxH, Color{180, 50, 50, 255});

    DrawText("RELATORIO FINAL DA CORRIDA:", boxX + 40, boxY + 25, 22, GOLD);

    DrawText(TextFormat("Pontuacao Final: %d pontos", stats.score), boxX + 40, boxY + 65, 24, RAYWHITE);
    DrawText(TextFormat("Distancia Percorrida: %d metros", static_cast<int>(stats.distance)), boxX + 40, boxY + 105, 20, LIGHTGRAY);
    DrawText(TextFormat("Alimentos Saudaveis Coletados: %d itens", stats.goodFoodCount), boxX + 40, boxY + 140, 20, LIME);
    DrawText(TextFormat("Comidas Ultraprocessadas Ingeridas: %d itens", stats.badFoodCount), boxX + 40, boxY + 175, 20, RED);
    DrawText(TextFormat("Recorde Salvo: %d pontos", stats.highScore), boxX + 40, boxY + 215, 22, GOLD);

    // Botões de Ação
    DrawCenteredText("Pressione [ENTER] para Jogar Novamente", 535, 25, LIME);
    DrawCenteredText("Pressione [ESC] para Voltar ao Menu Inicial", 575, 18, LIGHTGRAY);
}

void DrawPause()
{
    DrawRectangle(0, 0, Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT, Fade(BLACK, 0.65F));

    DrawCenteredText("JOGO PAUSADO", 240, 52, GOLD);
    DrawCenteredText("Pressione [P] para Retomar a Partida", 320, 24, RAYWHITE);
    DrawCenteredText("Pressione [ENTER] para Reiniciar a Partida", 365, 20, LIGHTGRAY);
    DrawCenteredText("Pressione [ESC] para Ir ao Menu", 405, 18, LIGHTGRAY);
}
