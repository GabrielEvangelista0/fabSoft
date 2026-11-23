#include "core/GameManager.h"
#include "core/Settings.h"
#include "ui/HUD.h"

int main()
{
    InitWindow(Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT, "Fuja das Calorias!");
    SetTargetFPS(Settings::TARGET_FPS);
    SetRandomSeed(static_cast<unsigned int>(GetTime()));

    InitHUD();

    GameManager game;
    while (!WindowShouldClose())
    {
        game.Update(GetFrameTime());
        game.Draw();
    }

    CloseHUD();
    CloseWindow();
    return 0;
}
