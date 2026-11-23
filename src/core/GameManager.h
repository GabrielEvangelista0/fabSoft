#pragma once

#include "raylib.h"

#include "core/AudioManager.h"
#include "core/EffectsSystem.h"
#include "core/GameStateMachine.h"
#include "entities/Player.h"
#include "systems/DifficultyManager.h"
#include "systems/SaveSystem.h"
#include "systems/ScoreSystem.h"
#include "world/FoodSystem.h"
#include "world/TrackSpawner.h"

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
    ~GameManager();

    void Update(float deltaTime);
    void Draw() const;

private:
    GameStateMachine stateMachine;
    Player       player;
    TrackSpawner track;
    FoodSystem   foodSystem;
    DifficultyManager difficulty;
    ScoreSystem  score;
    SaveSystem   saveSystem;
    AudioManager audio;

    Camera3D camera{};
    EffectsSystem effects;

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
};
