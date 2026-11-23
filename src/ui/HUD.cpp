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
        return Color{46, 204, 113, 255};   // Verde esmeralda
    }
    if (weight <= 70.0F)
    {
        return Color{241, 196, 15, 255};   // Amarelo sol
    }
    return Color{231, 76, 60, 255};        // Vermelho perigo
}

const char* GetWeightIcon(float weight)
{
    if (weight <= 40.0F) return "[FIT]";
    if (weight <= 70.0F) return "[!]";
    return "[!!]";
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
} // namespace

void InitHUD()
{
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

    const float drawX    = x - (textW * 0.5F);
    const float padding  = 6.0F;

    // Pílula de fundo para legibilidade máxima
    DrawRectangle(
        static_cast<int>(drawX - padding),
        static_cast<int>(y - padding * 0.5F),
        static_cast<int>(textW + padding * 2.0F),
        static_cast<int>(size + padding),
        Fade(Color{10, 10, 15, 255}, alpha * 0.72F));

    // Sombra forte + texto principal
    DrawTextCustom(text, drawX + 2.5F, y + 2.5F, size, Fade(BLACK,  alpha * 0.90F));
    DrawTextCustom(text, drawX,         y,         size, Fade(color,  alpha));
}

void DrawHUD(const Player& player, const HUDStats& stats)
{
    // =========================================================
    // PAINEL SUPERIOR COMPACTO (altura reduzida: 94 px)
    // =========================================================
    DrawRectangle(0, 0, Settings::WINDOW_WIDTH, 94, Fade(Color{12, 18, 30, 255}, 0.90F));
    DrawRectangle(0, 92, Settings::WINDOW_WIDTH, 2,  Color{52, 152, 219, 255});

    // ---- BLOCO ESQUERDO: Pontuação, Distância, Recorde ----
    DrawTextCustom(TextFormat("PONTOS: %05d", stats.score),    18.0F, 10.0F, 28.0F, RAYWHITE);
    DrawTextCustom(TextFormat("DIST: %d m",  static_cast<int>(stats.distance)), 18.0F, 48.0F, 18.0F, LIGHTGRAY);
    DrawTextCustom(TextFormat("REC: %d",     stats.highScore), 180.0F, 48.0F, 18.0F, GOLD);

    // ---- BLOCO CENTRAL: Barra de Peso/Saúde ----
    const float weight      = player.GetWeight();
    const float displayW    = stats.displayWeight;
    const Color statusColor = GetWeightBarColor(weight);

    constexpr float barX      = 400.0F;
    constexpr float barY      = 38.0F;
    constexpr float barWidth  = 350.0F;
    constexpr float barHeight = 26.0F;

    // Label + ícone de estado
    DrawTextCustom("PESO / SAUDE:", barX, 12.0F, 16.0F, Color{180, 185, 190, 255});
    DrawTextCustom(GetWeightIcon(weight), barX + 155.0F, 11.0F, 16.0F, statusColor);

    // Fundo da barra
    DrawRectangle(
        static_cast<int>(barX), static_cast<int>(barY),
        static_cast<int>(barWidth), static_cast<int>(barHeight),
        Color{25, 30, 42, 255});

    // Preenchimento animado (displayWeight → lerp suave no GameManager)
    const float progress = std::clamp(displayW / Settings::MAX_WEIGHT, 0.0F, 1.0F);
    DrawRectangle(
        static_cast<int>(barX), static_cast<int>(barY),
        static_cast<int>(barWidth * progress), static_cast<int>(barHeight),
        statusColor);

    // Brilho suave no topo da barra
    DrawRectangle(
        static_cast<int>(barX), static_cast<int>(barY),
        static_cast<int>(barWidth * progress), static_cast<int>(barHeight / 3),
        Fade(RAYWHITE, 0.12F));

    // Bordas e marcadores internos (40% e 70%)
    DrawRectangleLines(
        static_cast<int>(barX), static_cast<int>(barY),
        static_cast<int>(barWidth), static_cast<int>(barHeight), Fade(RAYWHITE, 0.6F));
    DrawLine(
        static_cast<int>(barX + barWidth * 0.40F), static_cast<int>(barY),
        static_cast<int>(barX + barWidth * 0.40F), static_cast<int>(barY + barHeight), Fade(RAYWHITE, 0.3F));
    DrawLine(
        static_cast<int>(barX + barWidth * 0.70F), static_cast<int>(barY),
        static_cast<int>(barX + barWidth * 0.70F), static_cast<int>(barY + barHeight), Fade(RAYWHITE, 0.3F));

    // Texto de peso
    DrawTextCustom(
        TextFormat("%.1f / %.0f kg", weight, Settings::MAX_WEIGHT),
        barX + barWidth + 12.0F, barY + 3.0F, 18.0F, RAYWHITE);

    // ---- BLOCO DIREITO: Comandos Rápidos (desaparece após 6 s) ----
    if (stats.commandsAlpha > 0.01F)
    {
        const float a    = stats.commandsAlpha;
        constexpr float cx = 880.0F;
        DrawTextCustom("COMANDOS:", cx, 8.0F,  14.0F, Fade(SKYBLUE,   a));
        DrawTextCustom("A/D ou Setas: Mudar Raia",          cx, 28.0F, 13.0F, Fade(RAYWHITE,  a));
        DrawTextCustom("W/ESP/CIMA: Pular | S/BAIXO: Agachar", cx, 46.0F, 13.0F, Fade(RAYWHITE,  a));
        DrawTextCustom(
            TextFormat("P: Pausa | M: Audio [%s]", stats.isMuted ? "MUDO" : "LIGADO"),
            cx, 66.0F, 13.0F, Fade(stats.isMuted ? ORANGE : LIME, a));
    }
    else
    {
        // Dica discreta permanente: só P e M
        const float a = 0.45F;
        DrawTextCustom(
            TextFormat("[P] Pausa  [M] Som: %s", stats.isMuted ? "Mudo" : "On"),
            880.0F, 36.0F, 13.0F, Fade(LIGHTGRAY, a));
    }

    // Flash de dano vermelho na tela
    if (stats.collisionFlash > 0.0F)
    {
        DrawRectangle(0, 0, Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT,
                      Fade(RED, stats.collisionFlash * 0.60F));
    }
}

void DrawMenu(int highScore, bool isMuted)
{
    DrawRectangleGradientV(0, 0, Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT,
                           Color{15, 26, 44, 255}, Color{6, 12, 22, 255});

    DrawCenteredTextCustom("FUJA DAS CALORIAS!", 65.0F + 3.0F, 64.0F, BLACK);
    DrawCenteredTextCustom("FUJA DAS CALORIAS!", 65.0F,        64.0F, Color{255, 215, 0, 255});
    DrawCenteredTextCustom("Endless Runner 3D de Nutricao e Sobrevivencia", 138.0F, 22.0F, SKYBLUE);

    constexpr int boxX = 140;
    constexpr int boxY = 180;
    constexpr int boxW = 1000;
    constexpr int boxH = 350;
    DrawRectangle(boxX, boxY, boxW, boxH, Fade(Color{22, 32, 52, 255}, 0.94F));
    DrawRectangleLines(boxX, boxY, boxW, boxH, Color{52, 152, 219, 255});

    DrawTextCustom("COMO JOGAR & REGRAS DO JOGO:", boxX + 35.0F, boxY + 20.0F, 22.0F, GOLD);

    DrawTextCustom("[+] COMIDAS BOAS (Maca Verde, Cenoura, Carne Grelhada):",
                   boxX + 35.0F, boxY + 58.0F, 19.0F, Color{46, 204, 113, 255});
    DrawTextCustom("    Reduzem seu peso corporal e concedem pontos de bonificacao!",
                   boxX + 35.0F, boxY + 84.0F, 17.0F, RAYWHITE);

    DrawTextCustom("[-] FAST-FOODS (Hamburguer, Batata Frita, Refri, Donut):",
                   boxX + 35.0F, boxY + 118.0F, 19.0F, Color{231, 76, 60, 255});
    DrawTextCustom("    Aumentam seu peso! Ao atingir 100 kg, ocorre colapso por obesidade (Game Over).",
                   boxX + 35.0F, boxY + 144.0F, 17.0F, RAYWHITE);

    DrawTextCustom("[*] MOVIMENTACAO E OBSTACULOS DINAMICOS:",
                   boxX + 35.0F, boxY + 180.0F, 19.0F, YELLOW);
    DrawTextCustom("    - Trocar de Raia: [A] / [D] ou [Setas Esquerda / Direita]",
                   boxX + 35.0F, boxY + 206.0F, 17.0F, RAYWHITE);
    DrawTextCustom("    - Pular obstaculos no chao ou pegar frutas altas: [W] / [ESPACO] / [SETA CIMA]",
                   boxX + 35.0F, boxY + 230.0F, 17.0F, RAYWHITE);
    DrawTextCustom("    - Agachar sob comidas suspensas no ar: [S] ou [SETA BAIXO]",
                   boxX + 35.0F, boxY + 254.0F, 17.0F, RAYWHITE);

    DrawTextCustom(TextFormat("MELHOR RECORDE SALVO: %d PONTOS", highScore),
                   boxX + 35.0F, boxY + 295.0F, 22.0F, GOLD);
    DrawTextCustom(TextFormat("[M] Audio: %s", isMuted ? "MUTADO" : "LIGADO"),
                   boxX + boxW - 220.0F, boxY + 298.0F, 18.0F, isMuted ? ORANGE : LIME);

    const float blink      = std::sin(static_cast<float>(GetTime()) * 5.0F);
    const Color promptColor = (blink > 0.0F) ? Color{46, 204, 113, 255} : RAYWHITE;
    DrawCenteredTextCustom(">> PRESSIONE [ENTER] PARA INICIAR A CORRIDA <<", 558.0F, 26.0F, promptColor);
    DrawCenteredTextCustom("Pressione [ESC] para sair da aplicacao",          602.0F, 17.0F, LIGHTGRAY);
}

void DrawGameOver(const HUDStats& stats)
{
    DrawRectangleGradientV(0, 0, Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT,
                           Color{38, 12, 14, 255}, Color{12, 4, 6, 255});

    DrawCenteredTextCustom("FIM DE JOGO!", 55.0F + 3.0F, 66.0F, BLACK);
    DrawCenteredTextCustom("FIM DE JOGO!", 55.0F,        66.0F, Color{231, 76, 60, 255});

    DrawCenteredTextCustom("Causa da Morte: Obesidade extrema decorrente de consumo de ultraprocessados!",
                           130.0F, 20.0F, RAYWHITE);

    if (stats.isNewRecord)
    {
        DrawCenteredTextCustom("*** NOVO RECORDE ALCANCADO! ***", 168.0F, 26.0F, GOLD);
    }

    constexpr int boxX = 260;
    constexpr int boxY = 212;
    constexpr int boxW = 760;
    constexpr int boxH = 290;
    DrawRectangle(boxX, boxY, boxW, boxH, Fade(Color{32, 22, 26, 255}, 0.94F));
    DrawRectangleLines(boxX, boxY, boxW, boxH, Color{192, 57, 43, 255});

    DrawTextCustom("RELATORIO FINAL DA CORRIDA:", boxX + 40.0F, boxY + 24.0F, 22.0F, GOLD);
    DrawTextCustom(TextFormat("Pontuacao Final: %d pontos",          stats.score),        boxX + 40.0F, boxY + 65.0F,  24.0F, RAYWHITE);
    DrawTextCustom(TextFormat("Distancia Percorrida: %d metros",     static_cast<int>(stats.distance)), boxX + 40.0F, boxY + 104.0F, 20.0F, LIGHTGRAY);
    DrawTextCustom(TextFormat("Alimentos Saudaveis Coletados: %d",   stats.goodFoodCount), boxX + 40.0F, boxY + 140.0F, 20.0F, Color{46, 204, 113, 255});
    DrawTextCustom(TextFormat("Ultraprocessados Ingeridos: %d",      stats.badFoodCount),  boxX + 40.0F, boxY + 176.0F, 20.0F, Color{231, 76,  60, 255});
    DrawTextCustom(TextFormat("Recorde Salvo: %d pontos",            stats.highScore),     boxX + 40.0F, boxY + 218.0F, 22.0F, GOLD);

    DrawCenteredTextCustom("Pressione [ENTER] para Jogar Novamente",    535.0F, 25.0F, Color{46, 204, 113, 255});
    DrawCenteredTextCustom("Pressione [ESC] para Voltar ao Menu Inicial", 575.0F, 18.0F, LIGHTGRAY);
}

void DrawPause()
{
    DrawRectangle(0, 0, Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT, Fade(BLACK, 0.70F));
    DrawCenteredTextCustom("JOGO PAUSADO",                     235.0F, 54.0F, GOLD);
    DrawCenteredTextCustom("Pressione [P] para Retomar",       315.0F, 24.0F, RAYWHITE);
    DrawCenteredTextCustom("Pressione [ENTER] para Reiniciar", 360.0F, 20.0F, LIGHTGRAY);
    DrawCenteredTextCustom("Pressione [ESC] para Ir ao Menu",  400.0F, 18.0F, LIGHTGRAY);
}
