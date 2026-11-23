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

    particles.reserve(128);
    popups.reserve(16);
}

void GameManager::Update(float deltaTime)
{
    if (IsKeyPressed(KEY_M))
    {
        audio.ToggleMute();
    }

    if (state == GameState::Menu)
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            StartGame();
        }
        return;
    }

    if (state == GameState::Paused)
    {
        if (IsKeyPressed(KEY_P))
        {
            state = GameState::Playing;
        }
        else if (IsKeyPressed(KEY_ENTER))
        {
            StartGame();
        }
        else if (IsKeyPressed(KEY_ESCAPE))
        {
            state = GameState::Menu;
        }
        return;
    }

    if (state == GameState::GameOver)
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            StartGame();
        }
        else if (IsKeyPressed(KEY_ESCAPE))
        {
            state = GameState::Menu;
        }
        return;
    }

    if (IsKeyPressed(KEY_P))
    {
        state = GameState::Paused;
        return;
    }

    UpdatePlaying(deltaTime);
}

void GameManager::Draw() const
{
    BeginDrawing();
    // Céu azul vibrante e límpido (limpa também o buffer de profundidade Z-buffer)
    ClearBackground(Color{120, 192, 248, 255});

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

    if (state == GameState::Playing || state == GameState::Paused)
    {
        BeginMode3D(camera);
        DrawTrack(track);
        foodSpawner.Draw();
        DrawParticles();
        player.Draw();
        EndMode3D();

        DrawHUD(player, stats);
        DrawPopups();

        if (state == GameState::Paused)
        {
            DrawPause();
        }
    }
    else if (state == GameState::Menu)
    {
        DrawMenu(highScore, audio.IsMuted());
    }
    else if (state == GameState::GameOver)
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
    foodSpawner.Reset();
    InitializeTrack(track);
    particles.clear();
    popups.clear();
    collisionFlash = 0.0F;
    isNewRecord    = false;
    wasJumping     = false;
    footstepTimer  = 0.0F;
    commandsTimer  = 6.0F;      // Exibe comandos nos primeiros 6s
    displayWeight  = 20.0F;
    state          = GameState::Playing;
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
        SpawnParticles({player.GetLaneX(), 0.06F, 0.0F}, Fade(LIGHTGRAY, 0.7F), 6, 0.08F);
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
            SpawnParticles({player.GetLaneX() + footOffset, 0.06F, -0.12F}, Fade(LIGHTGRAY, 0.5F), 2, 0.05F);
        }
    }

    // Partículas de suor se acima do peso
    if (player.GetWeightState() != WeightState::Normal && GetRandomValue(0, 100) < 15)
    {
        SpawnParticles(
            {player.GetLaneX() + static_cast<float>(GetRandomValue(-2, 2)) * 0.08F, 1.4F, 0.0F},
            SKYBLUE, 1, 0.05F);
    }

    // Decaimento do timer dos comandos rápidos
    if (commandsTimer > 0.0F)
    {
        commandsTimer -= deltaTime;
    }

    score.Update(deltaTime, speed);
    UpdateTrack(track, deltaTime, speed);
    foodSpawner.Update(
        deltaTime,
        speed,
        difficulty.GetBadFoodChance(distance),
        difficulty.GetSpawnInterval(distance));
    UpdateParticles(deltaTime);
    UpdatePopups(deltaTime);
    collisionFlash = std::max(0.0F, collisionFlash - deltaTime);

    // Peso exibido com lerp suave (animação da barra de saúde)
    const float actualWeight = player.GetWeight();
    displayWeight += (actualWeight - displayWeight) * std::min(1.0F, 5.0F * deltaTime);

    // Câmera segue suavemente o jogador lateralmente
    camera.position.x = player.GetLaneX() * 0.35F;
    camera.target.x   = player.GetLaneX() * 0.35F;

    // Colisões com alimentos
    for (const std::unique_ptr<Food>& food : foodSpawner.GetFoods())
    {
        if (food->IsActive() && CheckCollisionBoxes(player.GetBoundingBox(), food->GetBoundingBox()))
        {
            const Vector3 position = food->GetPosition();
            const bool isGoodFood  = (food->GetType() == FoodType::Good);

            food->ApplyTo(player);
            food->SetActive(false);

            if (isGoodFood)
            {
                const float comboMultiplier = score.RegisterGoodFood();
                const int points = static_cast<int>(
                    std::lround(static_cast<float>(food->GetScoreBonus()) * comboMultiplier));
                score.AddFoodBonus(points);
                audio.PlayCollectGood();

                // Partículas expressivas: sparkles verdes e amarelos
                SpawnParticles(position, Color{46, 204, 113, 255}, 14, 0.12F);
                SpawnParticles(position, Color{255, 235, 100, 255}, 4, 0.08F);

                SpawnPopup(
                    TextFormat(
                        "+%d pts  x%.2f  -%.0f kg (%s)",
                        points,
                        comboMultiplier,
                        food->GetWeightValue(),
                        food->GetDisplayName()),
                    Color{46, 204, 113, 255});
            }
            else
            {
                score.RegisterBadFood();
                audio.PlayCollectBad();
                collisionFlash = Settings::COLLISION_FLASH_DURATION;

                // Partículas de impacto: vermelho e laranja
                SpawnParticles(position, Color{231, 76, 60, 255}, 14, 0.12F);
                SpawnParticles(position, Color{255, 140, 20, 255}, 4, 0.08F);

                SpawnPopup(
                    TextFormat("+%.0f kg! (%s)", food->GetWeightValue(), food->GetDisplayName()),
                    Color{231, 76, 60, 255});
            }

            std::cout << "[Colisao] Item: " << food->GetDisplayName()
                      << " | Peso Atual: " << player.GetWeight() << " kg\n";
        }
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
        state = GameState::GameOver;
    }
}

void GameManager::UpdateParticles(float deltaTime)
{
    for (Particle& particle : particles)
    {
        particle.position.x += particle.velocity.x * deltaTime;
        particle.position.y += particle.velocity.y * deltaTime;
        particle.position.z += particle.velocity.z * deltaTime;
        particle.velocity.y -= Settings::GRAVITY * deltaTime * 0.35F;
        particle.lifetime   -= deltaTime;
    }

    particles.erase(
        std::remove_if(particles.begin(), particles.end(), [](const Particle& p)
        {
            return p.lifetime <= 0.0F;
        }),
        particles.end());
}

void GameManager::SpawnParticles(Vector3 position, Color color, int count, float size)
{
    for (int i = 0; i < count; ++i)
    {
        if (particles.size() >= 128)
        {
            break;
        }

        const float horizontal = static_cast<float>(GetRandomValue(-10, 10)) * 0.22F;
        const float vertical   = static_cast<float>(GetRandomValue(3, 9))  * 0.40F;
        const float depth      = static_cast<float>(GetRandomValue(-10, 10)) * 0.18F;

        particles.push_back({
            position,
            {horizontal, vertical, depth},
            Settings::PARTICLE_LIFETIME,
            Settings::PARTICLE_LIFETIME,
            color,
            size
        });
    }
}

void GameManager::DrawParticles() const
{
    for (const Particle& particle : particles)
    {
        const float alpha = std::clamp(particle.lifetime / particle.maxLifetime, 0.0F, 1.0F);
        DrawSphere(particle.position, particle.size, Fade(particle.color, alpha));
    }
}

void GameManager::UpdatePopups(float deltaTime)
{
    for (FloatingText& popup : popups)
    {
        popup.y       -= 45.0F * deltaTime;
        popup.lifetime -= deltaTime;
    }

    popups.erase(
        std::remove_if(popups.begin(), popups.end(), [](const FloatingText& p)
        {
            return p.lifetime <= 0.0F;
        }),
        popups.end());
}

void GameManager::SpawnPopup(const char* text, Color color)
{
    popups.push_back({
        std::string(text),
        static_cast<float>(Settings::WINDOW_WIDTH) * 0.5F,
        static_cast<float>(Settings::WINDOW_HEIGHT) * 0.45F,
        1.1F,
        color
    });
}

void GameManager::DrawPopups() const
{
    for (const FloatingText& popup : popups)
    {
        const float alpha = std::clamp(popup.lifetime / 1.1F, 0.0F, 1.0F);
        DrawFloatingMessage(popup.text.c_str(), popup.x, popup.y, 22.0F, popup.color, alpha);
    }
}
