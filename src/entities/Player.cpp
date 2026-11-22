#include "Player.h"

#include <algorithm>
#include <cmath>

#include "core/Settings.h"

Player::Player()
{
    Reset();
}

void Player::Reset()
{
    targetLane = 0;
    laneX = 0.0F;
    jumpHeight = 0.0F;
    verticalSpeed = 0.0F;
    runTime = 0.0F;
    weight = 20.0F;
    crouching = false;
    weightState = WeightState::Normal;
}

void Player::Update(float deltaTime)
{
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
    {
        targetLane = std::max(-1, targetLane - 1);
    }

    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
    {
        targetLane = std::min(1, targetLane + 1);
    }

    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP)) && jumpHeight <= 0.0F)
    {
        verticalSpeed = Settings::PLAYER_JUMP_SPEED;
    }

    crouching = IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S);
    const float targetX = static_cast<float>(targetLane) * Settings::LANE_WIDTH;
    const float laneChangeAmount = std::min(1.0F, Settings::PLAYER_LANE_CHANGE_SPEED * deltaTime);
    laneX += (targetX - laneX) * laneChangeAmount;

    if (jumpHeight > 0.0F || verticalSpeed > 0.0F)
    {
        jumpHeight += verticalSpeed * deltaTime;
        verticalSpeed -= Settings::GRAVITY * deltaTime;

        if (jumpHeight <= 0.0F)
        {
            jumpHeight = 0.0F;
            verticalSpeed = 0.0F;
        }
    }

    runTime += deltaTime;
}

void Player::Draw() const
{
    const float scale = (weightState == WeightState::Normal) ? 1.0F
        : (weightState == WeightState::Overweight) ? 1.25F : 1.55F;
    const float girthScale = (weightState == WeightState::Normal) ? 1.0F
        : (weightState == WeightState::Overweight) ? 1.3F : 1.7F;

    const float bob = (jumpHeight > 0.0F || crouching) ? 0.0F : 0.06F * std::sin(runTime * 14.0F);
    const float runCycle = std::sin(runTime * 14.0F);

    const Color shirtColor = (weightState == WeightState::Normal) ? Color{30, 144, 255, 255}
        : (weightState == WeightState::Overweight) ? Color{240, 160, 20, 255} : Color{220, 35, 35, 255};
    const Color skinColor = Color{240, 195, 160, 255};
    const Color shortsColor = Color{35, 40, 50, 255};

    // 1. TORSO (Tronco / Camiseta atlética)
    const float torsoY = jumpHeight + (crouching ? 0.38F : 0.95F) * scale + bob;
    const float torsoHeight = (crouching ? 0.40F : 0.70F) * scale;
    const float torsoWidth = 0.52F * girthScale;
    const float torsoDepth = 0.40F * girthScale;
    DrawCube({laneX, torsoY, 0.0F}, torsoWidth, torsoHeight, torsoDepth, shirtColor);
    DrawCubeWires({laneX, torsoY, 0.0F}, torsoWidth, torsoHeight, torsoDepth, DARKBLUE);

    // Barriga saliente proporcional ao ganho de peso (Overweight / Obese)
    if (weightState != WeightState::Normal)
    {
        const float bellyRadius = (weightState == WeightState::Overweight ? 0.28F : 0.42F) * scale;
        DrawSphere({laneX, torsoY - 0.08F * scale, 0.16F * girthScale}, bellyRadius, shirtColor);
    }

    // 2. CABEÇA E ROSTO
    const float headY = torsoY + (crouching ? 0.28F : 0.52F) * scale;
    const float headZ = (crouching ? 0.18F : 0.0F);
    const float headRadius = 0.22F * scale;
    DrawSphere({laneX, headY, headZ}, headRadius, skinColor);

    // Faixa esportiva de corrida (Headband)
    DrawCylinder({laneX, headY + 0.04F * scale, headZ}, headRadius * 1.05F, headRadius * 1.05F, 0.06F * scale, 12, RED);

    // Óculos escuros / Viseira esportiva
    DrawCube({laneX, headY, headZ + headRadius * 0.95F}, 0.26F * scale, 0.08F * scale, 0.06F * scale, Color{20, 20, 25, 255});

    // 3. BRAÇOS (Movimento dinâmico de corrida ou pulo)
    const float armY = torsoY + (crouching ? 0.05F : 0.15F) * scale;
    const float armSpan = (torsoWidth * 0.5F) + 0.08F;

    if (jumpHeight > 0.0F)
    {
        // Braços abertos em equilíbrio no ar
        DrawCube({laneX - armSpan - 0.1F, armY + 0.15F, 0.0F}, 0.12F, 0.35F, 0.12F, skinColor);
        DrawCube({laneX + armSpan + 0.1F, armY + 0.15F, 0.0F}, 0.12F, 0.35F, 0.12F, skinColor);
    }
    else if (crouching)
    {
        // Braços recolhidos ao agachar
        DrawCube({laneX - armSpan, armY - 0.1F, 0.1F}, 0.12F, 0.25F, 0.12F, skinColor);
        DrawCube({laneX + armSpan, armY - 0.1F, 0.1F}, 0.12F, 0.25F, 0.12F, skinColor);
    }
    else
    {
        // Balanço alternado dos braços
        const float leftArmZ = -runCycle * 0.22F;
        const float rightArmZ = runCycle * 0.22F;
        DrawCube({laneX - armSpan, armY, leftArmZ}, 0.12F, 0.40F, 0.12F, shirtColor);
        DrawCube({laneX - armSpan, armY - 0.22F, leftArmZ + 0.05F}, 0.10F, 0.18F, 0.10F, skinColor);

        DrawCube({laneX + armSpan, armY, rightArmZ}, 0.12F, 0.40F, 0.12F, shirtColor);
        DrawCube({laneX + armSpan, armY - 0.22F, rightArmZ + 0.05F}, 0.10F, 0.18F, 0.10F, skinColor);
    }

    // 4. PERNAS E TÊNIS (Alternância de passos ou recolhimento)
    const float legSpacing = 0.14F * girthScale;
    const float legBaseY = jumpHeight + (crouching ? 0.12F : 0.30F) * scale;

    if (jumpHeight > 0.0F)
    {
        // Pernas dobradas no salto
        DrawCube({laneX - legSpacing, legBaseY + 0.1F, -0.15F}, 0.14F, 0.35F, 0.14F, shortsColor);
        DrawCube({laneX + legSpacing, legBaseY + 0.1F, -0.15F}, 0.14F, 0.35F, 0.14F, shortsColor);
        DrawCube({laneX - legSpacing, legBaseY - 0.05F, -0.25F}, 0.15F, 0.10F, 0.24F, RAYWHITE);
        DrawCube({laneX + legSpacing, legBaseY - 0.05F, -0.25F}, 0.15F, 0.10F, 0.24F, RAYWHITE);
    }
    else if (crouching)
    {
        // Agachamento compacto rente ao solo
        DrawCube({laneX - legSpacing, legBaseY, 0.0F}, 0.14F, 0.20F, 0.30F, shortsColor);
        DrawCube({laneX + legSpacing, legBaseY, 0.0F}, 0.14F, 0.20F, 0.30F, shortsColor);
        DrawCube({laneX - legSpacing, 0.06F, -0.10F}, 0.16F, 0.10F, 0.24F, RAYWHITE);
        DrawCube({laneX + legSpacing, 0.06F, -0.10F}, 0.16F, 0.10F, 0.24F, RAYWHITE);
    }
    else
    {
        // Passadas dinâmicas de corrida
        const float leftLegZ = runCycle * 0.25F;
        const float rightLegZ = -runCycle * 0.25F;

        // Perna esquerda
        DrawCube({laneX - legSpacing, legBaseY, leftLegZ}, 0.14F, 0.55F, 0.14F, shortsColor);
        DrawCube({laneX - legSpacing, 0.07F, leftLegZ + 0.05F}, 0.16F, 0.12F, 0.26F, RAYWHITE);

        // Perna direita
        DrawCube({laneX + legSpacing, legBaseY, rightLegZ}, 0.14F, 0.55F, 0.14F, shortsColor);
        DrawCube({laneX + legSpacing, 0.07F, rightLegZ + 0.05F}, 0.16F, 0.12F, 0.26F, RAYWHITE);
    }
}

void Player::OnCollideGoodFood(float weightReduction)
{
    weight = std::max(0.0F, weight - weightReduction);
    UpdateWeightState();
}

void Player::OnCollideBadFood(float weightIncrease)
{
    weight = std::min(Settings::MAX_WEIGHT, weight + weightIncrease);
    UpdateWeightState();
}

float Player::GetWeight() const
{
    return weight;
}

float Player::GetLaneX() const
{
    return laneX;
}

WeightState Player::GetWeightState() const
{
    return weightState;
}

BoundingBox Player::GetBoundingBox() const
{
    const float scale = (weightState == WeightState::Normal) ? 1.0F
        : (weightState == WeightState::Overweight) ? 1.25F : 1.55F;
    const float girthScale = (weightState == WeightState::Normal) ? 1.0F
        : (weightState == WeightState::Overweight) ? 1.3F : 1.7F;

    const float height = (crouching ? Settings::PLAYER_HEIGHT * 0.35F : Settings::PLAYER_HEIGHT) * scale;
    const float width = Settings::PLAYER_WIDTH * girthScale;
    const float halfWidth = width * 0.5F;
    const float halfDepth = Settings::PLAYER_DEPTH * 0.5F;

    return {
        {laneX - halfWidth, jumpHeight, -halfDepth},
        {laneX + halfWidth, jumpHeight + height, halfDepth}
    };
}

bool Player::IsGameOver() const
{
    return weight >= Settings::MAX_WEIGHT;
}

bool Player::IsJumping() const
{
    return jumpHeight > 0.0F;
}

bool Player::IsCrouching() const
{
    return crouching;
}

void Player::UpdateWeightState()
{
    if (weight <= 40.0F)
    {
        weightState = WeightState::Normal;
    }
    else if (weight <= 70.0F)
    {
        weightState = WeightState::Overweight;
    }
    else
    {
        weightState = WeightState::Obese;
    }
}
