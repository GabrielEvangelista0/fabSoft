#include "core/GameManager.h"
#include "core/Settings.h"

int main()
{
    InitWindow(Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT, "Fuja das Calorias!");
    SetTargetFPS(Settings::TARGET_FPS);
    SetRandomSeed(static_cast<unsigned int>(GetTime()));

    GameManager game;
    while (!WindowShouldClose())
    {
        game.Update(GetFrameTime());
        game.Draw();
    }

    CloseWindow();
    return 0;
}
