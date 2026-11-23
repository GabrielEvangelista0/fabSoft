#pragma once

#include "raylib.h"

enum class WeightState
{
    Normal,
    Overweight,
    Obese
};

/**
 * @brief Entidade do Personagem Corredor.
 *
 * Gerencia posicionamento 3D nas 3 raias, movimentação com interpolação suave (Lerp),
 * física de salto com gravidade simulada, agachamento e alteração de forma/escala
 * decorrente do peso acumulado. Inclui squash & stretch e inclinação lateral.
 */
class Player
{
public:
    Player();

    void Update(float deltaTime, float worldSpeed);
    void Draw() const;
    void OnCollideGoodFood(float weightReduction);
    void OnCollideBadFood(float weightIncrease);
    void Reset();

    float GetWeight() const;
    float GetLaneX() const;
    float GetWorldZ() const;
    WeightState GetWeightState() const;
    BoundingBox GetBoundingBox() const;
    bool IsGameOver() const;
    bool IsJumping() const;
    bool IsCrouching() const;

private:
    int   targetLane    = 0;
    float laneX         = 0.0F;
    float prevLaneX     = 0.0F;    // Para calcular velocidade lateral
    float jumpHeight    = 0.0F;
    float verticalSpeed = 0.0F;
    float worldZ        = 0.0F;
    float runPhase      = 0.0F;
    float runSpeed      = 11.0F;
    float weight        = 20.0F;   // Começa saudável (20 kg)
    bool  crouching     = false;
    WeightState weightState = WeightState::Normal;

    // Game juice: animação visual
    float leanAngle   = 0.0F;    // Inclinação lateral (graus) ao trocar de raia
    float stretchTimer = 0.0F;   // Timer do efeito squash & stretch

    void UpdateWeightState();
};
