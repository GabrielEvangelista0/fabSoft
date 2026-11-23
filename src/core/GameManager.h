#pragma once

#include <string>
#include <vector>

#include "core/AudioManager.h"
#include "entities/Player.h"
#include "systems/DifficultyManager.h"
#include "systems/SaveSystem.h"
#include "systems/ScoreSystem.h"
#include "world/FoodSpawner.h"
#include "world/TrackSpawner.h"

enum class GameState
{
    Menu,
    Playing,
    Paused,
    GameOver
};

/**
 * @brief Controlador Principal do Fluxo do Jogo (State Machine).
 *
 * Coordena os subsistemas do jogo: Entidades, Física, Colisão, Áudio,
 * Pontuação, Persistência e Renderização gráfica 2D/3D.
 */
class GameManager
{
public:
    GameManager();

    void Update(float deltaTime);
    void Draw() const;

private:
    struct Particle
    {
        Vector3 position{};
        Vector3 velocity{};
        float   lifetime    = 0.0F;
        float   maxLifetime = 0.55F;
        Color   color       = LIME;
        float   size        = 0.10F;
    };

    struct FloatingText
    {
        std::string text;
        float x        = 0.0F;
        float y        = 0.0F;
        float lifetime = 0.0F;
        Color color    = WHITE;
    };

    GameState    state = GameState::Menu;
    Player       player;
    TrackSpawner track;
    FoodSpawner  foodSpawner;
    DifficultyManager difficulty;
    ScoreSystem  score;
    SaveSystem   saveSystem;
    AudioManager audio;

    Camera3D camera{};
    std::vector<Particle>    particles;
    std::vector<FloatingText> popups;

    int   highScore      = 0;
    float collisionFlash = 0.0F;
    bool  isNewRecord    = false;
    bool  wasJumping     = false;
    float footstepTimer  = 0.0F;

    // Timer do bloco de COMANDOS RAPIDOS (some após 6 s)
    float commandsTimer = 6.0F;

    // Peso exibido com interpolação suave (juice)
    mutable float displayWeight = 20.0F;

    void StartGame();
    void UpdatePlaying(float deltaTime);
    void UpdateParticles(float deltaTime);
    void SpawnParticles(Vector3 position, Color color = LIME, int count = 14, float size = 0.10F);
    void DrawParticles() const;

    void UpdatePopups(float deltaTime);
    void SpawnPopup(const char* text, Color color);
    void DrawPopups() const;
};
