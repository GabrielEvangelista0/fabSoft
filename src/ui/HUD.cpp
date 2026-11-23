#include "HUD.h"

#include <algorithm>
#include <cmath>

#include "core/Settings.h"

namespace
{
Font g_uiFont{};
bool g_fontLoaded = false;

Color GetWeightBarColor(float weight)
{
    if (weight <= 40.0F)
    {
        return Color{46, 204, 113, 255};  // Verde esmeralda vibrante
    }
    if (weight <= 70.0F)
    {
        return Color{241, 196, 15, 255};  // Amarelo sol
    }
    return Color{231, 76, 60, 255};       // Vermelho perigo intenso
}

void DrawTextCustom(const char* text, float x, float y, float fontSize, Color color, float spacing = 1.0F)
{
    if (g_fontLoaded)
    {
        DrawTextEx(g_uiFont, text, {x, y}, fontSize, spacing, color);
    }
    else
    {
        DrawText(text, static_cast<int>(x), static_cast<int>(y), static_cast<int>(fontSize), color);
    }
}

void DrawCenteredTextCustom(const char* text, float y, float fontSize, Color color, float spacing = 1.0F)
{
    float width = 0.0F;
    if (g_fontLoaded)
    {
        width = MeasureTextEx(g_uiFont, text, fontSize, spacing).x;
    }
    else
    {
        width = static_cast<float>(MeasureText(text, static_cast<int>(fontSize)));
    }
    const float x = (static_cast<float>(Settings::WINDOW_WIDTH) - width) * 0.5F;
    DrawTextCustom(text, x, y, fontSize, color, spacing);
}
}

void InitHUD()
{
    // Tenta carregar Arial / Arial Bold local ou do sistema para alta legibilidade
    const char* fontCandidates[] = {
        "assets/fonts/arialbd.ttf",
        "assets/fonts/arial.ttf",
        "C:/Windows/Fonts/arialbd.ttf",
        "C:/Windows/Fonts/arial.ttf"
    };

    for (const char* path : fontCandidates)
    {
        if (FileExists(path))
        {
            g_uiFont = LoadFontEx(path, 64, nullptr, 0);
            if (g_uiFont.texture.id > 0)
            {
                SetTextureFilter(g_uiFont.texture, TEXTURE_FILTER_BILINEAR);
                g_fontLoaded = true;
                break;
            }
        }
    }
}

void CloseHUD()
{
    if (g_fontLoaded)
    {
        UnloadFont(g_uiFont);
        g_fontLoaded = false;
    }
}

void DrawFloatingMessage(const char* text, float x, float y, float size, Color color, float alpha)
{
    float textW = 0.0F;
    if (g_fontLoaded)
    {
        textW = MeasureTextEx(g_uiFont, text, size, 1.0F).x;
    }
    else
    {
        textW = static_cast<float>(MeasureText(text, static_cast<int>(size)));
    }

    const float drawX = x - (textW * 0.5F);

    // Sombra do texto para contraste impecável
    DrawTextCustom(text, drawX + 2.0F, y + 2.0F, size, Fade(BLACK, alpha * 0.85F));
    DrawTextCustom(text, drawX, y, size, Fade(color, alpha));
}

void DrawHUD(const Player& player, const HUDStats& stats)
{
    // Painel superior semitransparente moderno (Glassmorphism)
    DrawRectangle(0, 0, Settings::WINDOW_WIDTH, 126, Fade(Color{18, 24, 38, 255}, 0.92F));
    DrawRectangle(0, 124, Settings::WINDOW_WIDTH, 2, Color{52, 152, 219, 255});

    // 1. Bloco Esquerdo: Pontuação e Distância
    DrawTextCustom("FUJA DAS CALORIAS", 28.0F, 14.0F, 22.0F, SKYBLUE);
    DrawTextCustom(TextFormat("PONTOS: %05d", stats.score), 28.0F, 44.0F, 26.0F, RAYWHITE);
    DrawTextCustom(TextFormat("DISTANCIA: %d m", static_cast<int>(stats.distance)), 28.0F, 78.0F, 19.0F, LIGHTGRAY);
    DrawTextCustom(TextFormat("RECORDE: %d", stats.highScore), 195.0F, 78.0F, 19.0F, GOLD);

    // 2. Bloco Central: Barra de Peso/Saúde
    constexpr float barX = 390.0F;
    constexpr float barY = 52.0F;
    constexpr float barWidth = 380.0F;
    constexpr float barHeight = 28.0F;

    const float weight = player.GetWeight();
    const char* statusLabel = (weight <= 40.0F) ? "FIT / SAUDAVEL"
        : (weight <= 70.0F) ? "SOBREPESO" : "OBESIDADE GRAVE!";
    const Color statusColor = GetWeightBarColor(weight);

    DrawTextCustom("PESO / SAUDE:", barX, 22.0F, 18.0F, RAYWHITE);
    DrawTextCustom(statusLabel, barX + 145.0F, 22.0F, 18.0F, statusColor);

    // Fundo da barra
    DrawRectangle(static_cast<int>(barX), static_cast<int>(barY), static_cast<int>(barWidth), static_cast<int>(barHeight), Color{35, 42, 54, 255});
    const float progress = std::clamp(weight / Settings::MAX_WEIGHT, 0.0F, 1.0F);
    DrawRectangle(static_cast<int>(barX), static_cast<int>(barY), static_cast<int>(barWidth * progress), static_cast<int>(barHeight), statusColor);
    DrawRectangleLines(static_cast<int>(barX), static_cast<int>(barY), static_cast<int>(barWidth), static_cast<int>(barHeight), RAYWHITE);

    // Marcadores internos da barra (40% e 70%)
    DrawLine(static_cast<int>(barX + barWidth * 0.40F), static_cast<int>(barY), static_cast<int>(barX + barWidth * 0.40F), static_cast<int>(barY + barHeight), Fade(RAYWHITE, 0.4F));
    DrawLine(static_cast<int>(barX + barWidth * 0.70F), static_cast<int>(barY), static_cast<int>(barX + barWidth * 0.70F), static_cast<int>(barY + barHeight), Fade(RAYWHITE, 0.4F));

    // Texto de peso em kg
    DrawTextCustom(TextFormat("%.1f / %.0f kg", weight, Settings::MAX_WEIGHT), barX + barWidth + 14.0F, barY + 4.0F, 20.0F, RAYWHITE);

    // 3. Bloco Direito: Controles Rápidos e Áudio
    constexpr float rightX = 890.0F;
    DrawTextCustom("COMANDOS RAPIDOS:", rightX, 16.0F, 16.0F, SKYBLUE);
    DrawTextCustom("A/D ou Setas: Mudar Raia", rightX, 38.0F, 15.0F, RAYWHITE);
    DrawTextCustom("W/ESPACO/CIMA: Pular | S/BAIXO: Agachar", rightX, 58.0F, 15.0F, RAYWHITE);
    DrawTextCustom(TextFormat("P: Pausar | M: Audio [%s]", stats.isMuted ? "MUDO" : "LIGADO"), rightX, 80.0F, 15.0F, stats.isMuted ? ORANGE : LIME);

    // Flash de dano vermelho na tela ao colidir com comida ruim
    if (stats.collisionFlash > 0.0F)
    {
        DrawRectangle(0, 0, Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT, Fade(RED, stats.collisionFlash * 0.65F));
    }
}

void DrawMenu(int highScore, bool isMuted)
{
    // Fundo degradê dinâmico
    DrawRectangleGradientV(0, 0, Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT, Color{15, 26, 44, 255}, Color{6, 12, 22, 255});

    // Título Principal com Sombra
    DrawCenteredTextCustom("FUJA DAS CALORIAS!", 65.0F + 3.0F, 64.0F, BLACK);
    DrawCenteredTextCustom("FUJA DAS CALORIAS!", 65.0F, 64.0F, Color{255, 215, 0, 255});
    DrawCenteredTextCustom("Endless Runner 3D de Nutricao e Sobrevivencia", 138.0F, 22.0F, SKYBLUE);

    // Painel Central com Guia do Jogo
    constexpr int boxX = 140;
    constexpr int boxY = 180;
    constexpr int boxW = 1000;
    constexpr int boxH = 350;
    DrawRectangle(boxX, boxY, boxW, boxH, Fade(Color{22, 32, 52, 255}, 0.94F));
    DrawRectangleLines(boxX, boxY, boxW, boxH, Color{52, 152, 219, 255});

    DrawTextCustom("COMO JOGAR & REGRAS DO JOGO:", boxX + 35.0F, boxY + 20.0F, 22.0F, GOLD);

    DrawTextCustom("[+] COMIDAS BOAS (Maca Verde, Cenoura, Carne Grelhada):", boxX + 35.0F, boxY + 58.0F, 19.0F, Color{46, 204, 113, 255});
    DrawTextCustom("    Reduzem seu peso corporal e concedem pontos de bonificacao!", boxX + 35.0F, boxY + 84.0F, 17.0F, RAYWHITE);

    DrawTextCustom("[-] FAST-FOODS (Hamburguer, Batata Frita, Refri, Donut):", boxX + 35.0F, boxY + 118.0F, 19.0F, Color{231, 76, 60, 255});
    DrawTextCustom("    Aumentam seu peso! Ao atingir 100 kg, ocorre colapso por obesidade (Game Over).", boxX + 35.0F, boxY + 144.0F, 17.0F, RAYWHITE);

    DrawTextCustom("[*] MOVIMENTACAO E OBSTACULOS DINAMICOS:", boxX + 35.0F, boxY + 180.0F, 19.0F, YELLOW);
    DrawTextCustom("    - Trocar de Raia: [A] / [D] ou [Setas Esquerda / Direita]", boxX + 35.0F, boxY + 206.0F, 17.0F, RAYWHITE);
    DrawTextCustom("    - Pular obstaculos no chao ou pegar frutas altas: [W] / [ESPACO] / [SETA CIMA]", boxX + 35.0F, boxY + 230.0F, 17.0F, RAYWHITE);
    DrawTextCustom("    - Agachar sob comidas suspensas no ar: [S] ou [SETA BAIXO]", boxX + 35.0F, boxY + 254.0F, 17.0F, RAYWHITE);

    DrawTextCustom(TextFormat("MELHOR RECORDE SALVO: %d PONTOS", highScore), boxX + 35.0F, boxY + 295.0F, 22.0F, GOLD);
    DrawTextCustom(TextFormat("[M] Audio: %s", isMuted ? "MUTADO" : "LIGADO"), boxX + boxW - 220.0F, boxY + 298.0F, 18.0F, isMuted ? ORANGE : LIME);

    // Botão de Início Piscante
    const float blink = std::sin(static_cast<float>(GetTime()) * 5.0F);
    const Color promptColor = (blink > 0.0F) ? Color{46, 204, 113, 255} : RAYWHITE;
    DrawCenteredTextCustom(">> PRESSIONE [ENTER] PARA INICIAR A CORRIDA <<", 558.0F, 26.0F, promptColor);
    DrawCenteredTextCustom("Pressione [ESC] para sair da aplicacao", 602.0F, 17.0F, LIGHTGRAY);
}

void DrawGameOver(const HUDStats& stats)
{
    DrawRectangleGradientV(0, 0, Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT, Color{38, 12, 14, 255}, Color{12, 4, 6, 255});

    DrawCenteredTextCustom("FIM DE JOGO!", 55.0F + 3.0F, 66.0F, BLACK);
    DrawCenteredTextCustom("FIM DE JOGO!", 55.0F, 66.0F, Color{231, 76, 60, 255});

    DrawCenteredTextCustom("Causa da Morte: Obesidade extrema decorrente de consumo de ultraprocessados!", 130.0F, 20.0F, RAYWHITE);

    if (stats.isNewRecord)
    {
        DrawCenteredTextCustom("*** NOVO RECORDE ALCANCADO! ***", 168.0F, 26.0F, GOLD);
    }

    // Painel de Estatísticas da Partida
    constexpr int boxX = 260;
    constexpr int boxY = 212;
    constexpr int boxW = 760;
    constexpr int boxH = 290;
    DrawRectangle(boxX, boxY, boxW, boxH, Fade(Color{32, 22, 26, 255}, 0.94F));
    DrawRectangleLines(boxX, boxY, boxW, boxH, Color{192, 57, 43, 255});

    DrawTextCustom("RELATORIO FINAL DA CORRIDA:", boxX + 40.0F, boxY + 24.0F, 22.0F, GOLD);

    DrawTextCustom(TextFormat("Pontuacao Final: %d pontos", stats.score), boxX + 40.0F, boxY + 65.0F, 24.0F, RAYWHITE);
    DrawTextCustom(TextFormat("Distancia Percorrida: %d metros", static_cast<int>(stats.distance)), boxX + 40.0F, boxY + 104.0F, 20.0F, LIGHTGRAY);
    DrawTextCustom(TextFormat("Alimentos Saudaveis Coletados: %d itens", stats.goodFoodCount), boxX + 40.0F, boxY + 140.0F, 20.0F, Color{46, 204, 113, 255});
    DrawTextCustom(TextFormat("Comidas Ultraprocessadas Ingeridas: %d itens", stats.badFoodCount), boxX + 40.0F, boxY + 176.0F, 20.0F, Color{231, 76, 60, 255});
    DrawTextCustom(TextFormat("Recorde Salvo: %d pontos", stats.highScore), boxX + 40.0F, boxY + 218.0F, 22.0F, GOLD);

    // Botões de Ação
    DrawCenteredTextCustom("Pressione [ENTER] para Jogar Novamente", 535.0F, 25.0F, Color{46, 204, 113, 255});
    DrawCenteredTextCustom("Pressione [ESC] para Voltar ao Menu Inicial", 575.0F, 18.0F, LIGHTGRAY);
}

void DrawPause()
{
    DrawRectangle(0, 0, Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT, Fade(BLACK, 0.70F));

    DrawCenteredTextCustom("JOGO PAUSADO", 235.0F, 54.0F, GOLD);
    DrawCenteredTextCustom("Pressione [P] para Retomar a Partida", 315.0F, 24.0F, RAYWHITE);
    DrawCenteredTextCustom("Pressione [ENTER] para Reiniciar", 360.0F, 20.0F, LIGHTGRAY);
    DrawCenteredTextCustom("Pressione [ESC] para Ir ao Menu", 400.0F, 18.0F, LIGHTGRAY);
}
