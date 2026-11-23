#include "GameManager.h"

#include <algorithm>
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

    // Céu gradiente: azul celeste no topo → laranja-âmbar no horizonte
    DrawRectangleGradientV(
        0, 0, Settings::WINDOW_WIDTH, Settings::WINDOW_HEIGHT,
        Color{135, 185, 230, 255},   // Azul suave/manhã no topo
        Color{235, 190, 130, 255});  // Âmbar quente no horizonte

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

        // --- NÉVOA ATMOSFÉRICA NO HORIZONTE (pós-3D, pré-HUD) ---
        // Gradiente de baixo para cima: haze âmbar/azul do horizonte
        DrawRectangleGradientV(
            0,
            Settings::WINDOW_HEIGHT / 3,
            Settings::WINDOW_WIDTH,
            Settings::WINDOW_HEIGHT / 5,
            Fade(Color{210, 190, 160, 255}, 0.0F),
            Fade(Color{210, 190, 160, 255}, 0.0F));
        // Haze leve no ponto de fuga (centro vertical ~40%)
        DrawRectangleGradientV(
            0,
            Settings::WINDOW_HEIGHT / 4,
            Settings::WINDOW_WIDTH,
            Settings::WINDOW_HEIGHT / 6,
            Fade(Color{220, 200, 170, 255}, 0.0F),
            Fade(Color{220, 200, 170, 255}, 0.22F));

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
    commandsTimer  = 6.0F;      // Reinicia o timer dos COMANDOS RAPIDOS
    displayWeight  = 20.0F;
    state          = GameState::Playing;
}

void GameManager::UpdatePlaying(float deltaTime)
{
    player.Update(deltaTime);

    if (!wasJumping && player.IsJumping())
    {
        audio.PlayJump();
        // Poeira no impulso do salto — 3 partículas cinza leve
        SpawnParticles({player.GetLaneX(), 0.06F, 0.0F}, Fade(Color{200, 195, 185, 255}, 0.8F), 4, 0.07F);
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
            SpawnParticles({player.GetLaneX() + footOffset, 0.06F, -0.12F}, Fade(Color{195, 185, 170, 255}, 0.55F), 3, 0.045F);
        }
    }

    // Partículas de suor se acima do peso
    if (player.GetWeightState() != WeightState::Normal && GetRandomValue(0, 100) < 15)
    {
        SpawnParticles(
            {player.GetLaneX() + static_cast<float>(GetRandomValue(-2, 2)) * 0.08F, 1.4F, 0.0F},
            Color{140, 190, 230, 255}, 1, 0.05F);
    }

    // Decaimento do timer dos COMANDOS RAPIDOS
    if (commandsTimer > 0.0F)
    {
        commandsTimer -= deltaTime;
    }

    const float distance = score.GetDistance();
    const float weightSpeedMultiplier = (player.GetWeightState() == WeightState::Normal) ? 1.0F
        : (player.GetWeightState() == WeightState::Overweight) ? 0.90F : 0.78F;
    const float speed = difficulty.GetSpeed(distance) * weightSpeedMultiplier;

    score.Update(deltaTime, speed);
    UpdateTrack(track, deltaTime, speed);
    foodSpawner.Update(deltaTime, speed, difficulty.GetBadFoodChance(distance));
    UpdateParticles(deltaTime);
    UpdatePopups(deltaTime);
    collisionFlash = std::max(0.0F, collisionFlash - deltaTime);

    // Peso exibido com lerp suave (juice da barra de vida)
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
                score.AddFoodBonus(food->GetScoreBonus());
                score.RegisterGoodFood();
                audio.PlayCollectGood();

                // Partículas de coleta boa: sparkles verdes, amarelas e brancas
                SpawnParticles(position, Color{ 60, 220,  90, 255}, 10, 0.10F);
                SpawnParticles(position, Color{255, 240, 100, 255},  4, 0.07F);
                SpawnParticles(position, Color{240, 255, 240, 255},  3, 0.06F);

                SpawnPopup(
                    TextFormat("+%d pts  -%.0f kg (%s)", food->GetScoreBonus(), food->GetWeightValue(), food->GetDisplayName()),
                    Color{46, 220, 110, 255});
            }
            else
            {
                score.RegisterBadFood();
                audio.PlayCollectBad();
                collisionFlash = Settings::COLLISION_FLASH_DURATION;

                // Partículas de dano: vermelho + laranja (onda de impacto)
                SpawnParticles(position, Color{240,  70,  50, 255}, 10, 0.11F);
                SpawnParticles(position, Color{255, 160,  40, 255},  5, 0.08F);
                SpawnParticles(position, Color{100,  20,  10, 255},  3, 0.07F);

                SpawnPopup(
                    TextFormat("+%.0f kg! (%s)", food->GetWeightValue(), food->GetDisplayName()),
                    Color{235, 70, 50, 255});
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
        if (particles.size() >= 192)
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
        popup.y       -= 50.0F * deltaTime;
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
        static_cast<float>(Settings::WINDOW_HEIGHT) * 0.42F,
        1.2F,
        color
    });
}

void GameManager::DrawPopups() const
{
    for (const FloatingText& popup : popups)
    {
        const float alpha = std::clamp(popup.lifetime / 1.2F, 0.0F, 1.0F);
        // Texto principal maior com sombra forte
        DrawFloatingMessage(popup.text.c_str(), popup.x, popup.y, 26.0F, popup.color, alpha);
    }
}
