#pragma once

enum class GameState
{
    Menu,
    Playing,
    Paused,
    GameOver
};

enum class GameAction
{
    None,
    Start,
    TogglePause,
    Resume,
    Menu
};

class GameStateMachine
{
public:
    GameState GetState() const;
    void SetState(GameState state);
    void ResetForNewGame();
    GameAction Update(float deltaTime);

private:
    GameState state = GameState::Menu;
};
