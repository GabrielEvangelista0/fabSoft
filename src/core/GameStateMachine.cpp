#include "GameStateMachine.h"

#include "raylib.h"

GameState GameStateMachine::GetState() const
{
    return state;
}

void GameStateMachine::SetState(GameState newState)
{
    state = newState;
}

void GameStateMachine::ResetForNewGame()
{
    state = GameState::Playing;
}

GameAction GameStateMachine::Update(float)
{
    if (IsKeyPressed(KEY_M))
    {
        // Audio mute remains handled by GameManager to preserve the subsystem boundary.
    }

    if (state == GameState::Menu)
    {
        return IsKeyPressed(KEY_ENTER) ? GameAction::Start : GameAction::None;
    }

    if (state == GameState::Paused)
    {
        if (IsKeyPressed(KEY_P))
        {
            return GameAction::Resume;
        }
        if (IsKeyPressed(KEY_ENTER))
        {
            return GameAction::Start;
        }
        if (IsKeyPressed(KEY_ESCAPE))
        {
            return GameAction::Menu;
        }
        return GameAction::None;
    }

    if (state == GameState::GameOver)
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            return GameAction::Start;
        }
        if (IsKeyPressed(KEY_ESCAPE))
        {
            return GameAction::Menu;
        }
        return GameAction::None;
    }

    return IsKeyPressed(KEY_P) ? GameAction::TogglePause : GameAction::None;
}
