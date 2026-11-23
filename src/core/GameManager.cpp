#include "GameManager.h"

#include <algorithm>
#include <cmath>
#include <iostream>

#include "core/Settings.h"
#include "ui/HUD.h"

GameManager::GameManager()
    : highScore(saveSystem.LoadHighScore())
{
    InitializeTrack(track);
    camera.position   = {0.0F, 5.2F, -9.5F};
    camera.target     = {0.0F, 1.2F,  4.0F};
    camera.up         = {0.0F, 1.0F,  0.0F};
    camera.fovy       = 55.0F;
    camera.projection = CAMERA_PERSPECTIVE;

    effects.Reset();
}
GameManager::~GameManager()
{
    UnloadTrack(track);
}

void GameManager::Update(float deltaTime)
{
    if (IsKeyPressed(KEY_M))
    {
        audio.ToggleMute();
    }

    const GameAction action = stateMachine.Update(deltaTime);
    switch (action)
    {
        case GameAction::Start:
            StartGame();
            return;
        case GameAction::TogglePause:
            stateMachine.SetState(GameState::Paused);
            return;
        case GameAction::Resume:
            stateMachine.SetState(GameState::Playing);
            return;
        case GameAction::Menu:
            stateMachine.SetState(GameState::Menu);
            return;
        case GameAction::None:
            break;
    }

    if (stateMachine.GetState() == GameState::Playing)
    {
        UpdatePlaying(deltaTime);
    }
}

void GameManager::Draw() const
{
    BeginDrawing();
    const float distance = score.GetDistance();
    const float weatherPhase = std::clamp(distance / 2400.0F, 0.0F, 1.0F);

    const Color dayHorizon{178, 220, 238, 255};
    const Color duskHorizon{238, 168, 128, 255};
    const Color nightHorizon{62, 82, 118, 255};
    const Color dayTop{82, 158, 218, 255};
    const Color duskTop{130, 120, 188, 255};
    const Color nightTop{18, 34, 68, 255};

    Color horizon{};
    Color top{};
    if (weatherPhase < 0.55F)
    {
        const float t = weatherPhase / 0.55F;
        horizon = {
            static_cast<unsigned char>(dayHorizon.r + (duskHorizon.r - dayHorizon.r) * t),
            static_cast<unsigned char>(dayHorizon.g + (duskHorizon.g - dayHorizon.g) * t),
            static_cast<unsigned char>(dayHorizon.b + (duskHorizon.b - dayHorizon.b) * t),
            255};
        top = {
            static_cast<unsigned char>(dayTop.r + (duskTop.r - dayTop.r) * t),
            static_cast<unsigned char>(dayTop.g + (duskTop.g - dayTop.g) * t),
            static_cast<unsigned char>(dayTop.b + (duskTop.b - dayTop.b) * t),
            255};
    }
    else
    {
        const float t = (weatherPhase - 0.55F) / 0.45F;
        horizon = {
            static_cast<unsigned char>(duskHorizon.r + (nightHorizon.r - duskHorizon.r) * t),
            static_cast<unsigned char>(duskHorizon.g + (nightHorizon.g - duskHorizon.g) * t),
            static_cast<unsigned char>(duskHorizon.b + (nightHorizon.b - duskHorizon.b) * t),
            255};
        top = {
            static_cast<unsigned char>(duskTop.r + (nightTop.r - duskTop.r) * t),
            static_cast<unsigned char>(duskTop.g + (nightTop.g - duskTop.g) * t),
            static_cast<unsigned char>(duskTop.b + (nightTop.b - duskTop.b) * t),
            255};
    }

    ClearBackground(horizon);
    const int horizonY = Settings::WINDOW_HEIGHT * 58 / 100;
    DrawRectangleGradientV(
        0,
        0,
        Settings::WINDOW_WIDTH,
        horizonY,
        top,
        horizon);

    HUDStats stats{};
    stats.score          = score.GetScore();
    stats.distance       = score.GetDistance();
    stats.highScore      = highScore;
    stats.collisionFlash = collisionFlash;
    stats.isMuted        = audio.IsMuted();
    stats.isNewRecord    = isNewRecord;
    stats.goodFoodCount  = score.GetGoodFoodCount();
    stats.badFoodCount   = score.GetBadFoodCount();
    stats.commandsAlpha  = std::clamp(commandsTimer / 1.5F, 0.0F, 1.0F);
    stats.displayWeight  = displayWeight;

    if (stateMachine.GetState() == GameState::Playing ||
        stateMachine.GetState() == GameState::Paused)
    {
        BeginMode3D(camera);
        DrawTrack(track);
        foodSystem.Draw();
        effects.Draw();
        player.Draw();
        EndMode3D();

        DrawHUD(player, stats);
        effects.DrawPopups();

        if (stateMachine.GetState() == GameState::Paused)
        {
            DrawPause();
        }
    }
    else if (stateMachine.GetState() == GameState::Menu)
    {
        DrawMenu(highScore, audio.IsMuted());
    }
    else if (stateMachine.GetState() == GameState::GameOver)
    {
        DrawGameOver(stats);
    }

    DrawFPS(Settings::WINDOW_WIDTH - 90, Settings::WINDOW_HEIGHT - 25);
    EndDrawing();
}

void GameManager::StartGame()
{
    player.Reset();
    score.Reset();
    foodSystem.Reset();
    UnloadTrack(track);
    InitializeTrack(track);
    effects.Reset();
    collisionFlash = 0.0F;
    isNewRecord    = false;
    wasJumping     = false;
    footstepTimer  = 0.0F;
    commandsTimer  = 6.0F;      // Exibe comandos nos primeiros 6s
    displayWeight  = 20.0F;
    stateMachine.ResetForNewGame();
}

void GameManager::UpdatePlaying(float deltaTime)
{
    const float distance = score.GetDistance();
    const float weightSpeedMultiplier = (player.GetWeightState() == WeightState::Normal) ? 1.0F
        : (player.GetWeightState() == WeightState::Overweight) ? 0.90F : 0.78F;
    const float speed = difficulty.GetSpeed(distance) * weightSpeedMultiplier;
    player.Update(deltaTime, speed);

    if (!wasJumping && player.IsJumping())
    {
        audio.PlayJump();
        // Poeira no impulso do salto
        effects.SpawnParticles({player.GetLaneX(), 0.06F, player.GetWorldZ()}, Fade(LIGHTGRAY, 0.7F), 6, 0.08F);
    }
    wasJumping = player.IsJumping();

    // Partículas de poeira durante a corrida (passo a passo)
    if (!player.IsJumping() && !player.IsCrouching())
    {
        footstepTimer += deltaTime;
        if (footstepTimer >= 0.14F)
        {
            footstepTimer = 0.0F;
            const float footOffset = (std::sin(static_cast<float>(GetTime()) * 14.0F) > 0.0F) ? -0.14F : 0.14F;
            effects.SpawnParticles({player.GetLaneX() + footOffset, 0.06F, player.GetWorldZ() - 0.12F}, Fade(LIGHTGRAY, 0.5F), 2, 0.05F);
        }
    }

    // Partículas de suor se acima do peso
    if (player.GetWeightState() != WeightState::Normal && GetRandomValue(0, 100) < 15)
    {
        effects.SpawnParticles(
            {player.GetLaneX() + static_cast<float>(GetRandomValue(-2, 2)) * 0.08F, 1.4F, player.GetWorldZ()},
            SKYBLUE, 1, 0.05F);
    }

    // Decaimento do timer dos comandos rápidos
    if (commandsTimer > 0.0F)
    {
        commandsTimer -= deltaTime;
    }

    score.Update(deltaTime, speed);
    UpdateTrack(track, deltaTime, speed, player.GetWorldZ());
    foodSystem.Update(
        deltaTime,
        speed,
        difficulty.GetBadFoodChance(distance),
        difficulty.GetSpawnInterval(distance),
        player.GetWorldZ());
    effects.Update(deltaTime);
    collisionFlash = std::max(0.0F, collisionFlash - deltaTime);

    // Peso exibido com lerp suave (animação da barra de saúde)
    const float actualWeight = player.GetWeight();
    displayWeight += (actualWeight - displayWeight) * std::min(1.0F, 5.0F * deltaTime);

    // Câmera segue suavemente o jogador lateralmente
    camera.position.x = player.GetLaneX() * 0.35F;
    camera.target.x   = player.GetLaneX() * 0.35F;
    camera.position.z = player.GetWorldZ() - 9.5F;
    camera.target.z   = player.GetWorldZ() + 4.0F;

    // Colisões e regras de bônus ficam no FoodSystem.
    foodSystem.ResolveCollisions(player, score);
    for (const FoodSystem::CollisionEvent& event : foodSystem.GetCollisionEvents())
    {
        if (event.isGood)
        {
            audio.PlayCollectGood();
            effects.SpawnParticles(event.position, Color{46, 204, 113, 255}, 14, 0.12F);
            effects.SpawnParticles(event.position, Color{255, 235, 100, 255}, 4, 0.08F);
            effects.SpawnPopup(
                TextFormat("+%d pts  x%.2f  -%.0f kg (%s)",
                           event.points, event.comboMultiplier,
                           event.weightValue, event.displayName),
                Color{46, 204, 113, 255});
        }
        else
        {
            audio.PlayCollectBad();
            collisionFlash = Settings::COLLISION_FLASH_DURATION;
            effects.SpawnParticles(event.position, Color{231, 76, 60, 255}, 14, 0.12F);
            effects.SpawnParticles(event.position, Color{255, 140, 20, 255}, 4, 0.08F);
            effects.SpawnPopup(
                TextFormat("+%.0f kg! (%s)", event.weightValue, event.displayName),
                Color{231, 76, 60, 255});
        }

        std::cout << "[Colisao] Item: " << event.displayName
                  << " | Peso Atual: " << player.GetWeight() << " kg\n";
    }

    if (player.IsGameOver())
    {
        audio.PlayGameOver();
        if (score.GetScore() > highScore)
        {
            highScore    = score.GetScore();
            isNewRecord  = true;
            saveSystem.SaveHighScore(highScore);
        }
        stateMachine.SetState(GameState::GameOver);
    }
}
