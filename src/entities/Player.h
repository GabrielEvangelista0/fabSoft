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
 * decorrente do peso acumulado.
 */
class Player
{
public:
    Player();

    void Update(float deltaTime);
    void Draw() const;
    void OnCollideGoodFood(float weightReduction);
    void OnCollideBadFood(float weightIncrease);
    void Reset();

    float GetWeight() const;
    float GetLaneX() const;
    WeightState GetWeightState() const;
    BoundingBox GetBoundingBox() const;
    bool IsGameOver() const;
    bool IsJumping() const;
    bool IsCrouching() const;

private:
    int targetLane = 0;
    float laneX = 0.0F;
    float jumpHeight = 0.0F;
    float verticalSpeed = 0.0F;
    float runTime = 0.0F;
    float weight = 20.0F; // Começa saudável (20 kg)
    bool crouching = false;
    WeightState weightState = WeightState::Normal;

    void UpdateWeightState();
};
