#pragma once

#include "entities/Player.h"

struct HUDStats
{
    int score = 0;
    float distance = 0.0F;
    int highScore = 0;
    float collisionFlash = 0.0F;
    bool isMuted = false;
    bool isNewRecord = false;
    int goodFoodCount = 0;
    int badFoodCount = 0;
};

void InitHUD();
void CloseHUD();
void DrawHUD(const Player& player, const HUDStats& stats);
void DrawMenu(int highScore, bool isMuted);
void DrawGameOver(const HUDStats& stats);
void DrawPause();
void DrawFloatingMessage(const char* text, float x, float y, float size, Color color, float alpha);
