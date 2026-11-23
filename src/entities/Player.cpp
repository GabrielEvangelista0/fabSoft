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
    targetLane      = 0;
    laneX           = 0.0F;
    jumpHeight      = 0.0F;
    verticalSpeed   = 0.0F;
    runTime         = 0.0F;
    weight          = 20.0F;
    crouching       = false;
    weightState     = WeightState::Normal;
    leanAngle       = 0.0F;
    stretchTimer    = 0.0F;
    prevLaneX       = 0.0F;
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

    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && jumpHeight <= 0.0F)
    {
        verticalSpeed = Settings::PLAYER_JUMP_SPEED;
        stretchTimer  = 0.18F;  // Squash & stretch no impulso do salto
    }

    crouching = IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S);

    prevLaneX = laneX;
    const float targetX = static_cast<float>(-targetLane) * Settings::LANE_WIDTH;
    const float laneChangeAmount = std::min(1.0F, Settings::PLAYER_LANE_CHANGE_SPEED * deltaTime);
    laneX += (targetX - laneX) * laneChangeAmount;

    // Inclinação lateral ao trocar de faixa
    const float lateralVel = (laneX - prevLaneX) / deltaTime;
    const float targetLean = std::clamp(-lateralVel * 12.0F, -18.0F, 18.0F);
    leanAngle += (targetLean - leanAngle) * std::min(1.0F, 10.0F * deltaTime);

    if (jumpHeight > 0.0F || verticalSpeed > 0.0F)
    {
        jumpHeight    += verticalSpeed * deltaTime;
        verticalSpeed -= Settings::GRAVITY * deltaTime;

        if (jumpHeight <= 0.0F)
        {
            jumpHeight    = 0.0F;
            verticalSpeed = 0.0F;
            stretchTimer  = 0.12F;  // Squash na aterrissagem
        }
    }

    // Decaimento do timer de squash & stretch
    if (stretchTimer > 0.0F)
    {
        stretchTimer -= deltaTime;
    }

    runTime += deltaTime;
}

void Player::Draw() const
{
    const float scale = (weightState == WeightState::Normal) ? 1.0F
        : (weightState == WeightState::Overweight) ? 1.25F : 1.55F;
    const float girthScale = (weightState == WeightState::Normal) ? 1.0F
        : (weightState == WeightState::Overweight) ? 1.3F : 1.7F;

    // Squash & Stretch: escala vertical/horizontal animada
    float stretchX = 1.0F;
    float stretchY = 1.0F;
    if (stretchTimer > 0.0F)
    {
        const float t = stretchTimer / 0.18F;
        if (verticalSpeed > 0.0F || jumpHeight > 0.1F)
        {
            // No ar: estica para cima (fino e alto)
            stretchX = 1.0F - t * 0.12F;
            stretchY = 1.0F + t * 0.14F;
        }
        else
        {
            // Aterrissagem / agachamento: achata (largo e baixo)
            stretchX = 1.0F + t * 0.18F;
            stretchY = 1.0F - t * 0.14F;
        }
    }

    const float bob = (jumpHeight > 0.0F || crouching) ? 0.0F : 0.06F * std::sin(runTime * 14.0F);

    // Amplitude de corrida aumenta com o peso (pesado = mais suado/desengonçado)
    const float speedFactor = (weightState == WeightState::Normal) ? 1.0F
        : (weightState == WeightState::Overweight) ? 1.15F : 1.30F;
    const float runCycle = std::sin(runTime * 14.0F) * speedFactor;

    const Color shirtColor = (weightState == WeightState::Normal) ? Color{30, 144, 255, 255}
        : (weightState == WeightState::Overweight) ? Color{240, 160, 20, 255} : Color{220, 35, 35, 255};
    const Color skinColor    = Color{240, 195, 160, 255};
    const Color shortsColor  = Color{35, 40, 50, 255};

    // ------------------------------------------------
    // SOMBRA NO CHÃO — elipse semi-transparente
    // ------------------------------------------------
    // Sombra encolhe quando o jogador salta
    const float shadowScale = std::max(0.3F, 1.0F - jumpHeight * 0.18F);
    DrawCylinder(
        {laneX + jumpHeight * 0.15F, 0.01F, 0.0F},
        0.42F * girthScale * shadowScale,
        0.42F * girthScale * shadowScale,
        0.012F, 12,
        Fade(BLACK, 0.22F * shadowScale));

    // ------------------------------------------------
    // INCLINAÇÃO LATERAL (lean) com rlgl
    // ------------------------------------------------
    // Aplicada via deslocamento do ponto de origem do torso
    const float leanOffset = leanAngle * 0.004F;  // Offset horizontal sutil

    // 1. TORSO
    const float torsoY      = jumpHeight + (crouching ? 0.38F : 0.95F) * scale * stretchY + bob;
    const float torsoHeight = (crouching ? 0.40F : 0.70F) * scale * stretchY;
    const float torsoWidth  = 0.52F * girthScale * stretchX;
    const float torsoDepth  = 0.40F * girthScale * stretchX;
    const float leanedX     = laneX + leanOffset;
    DrawCube({leanedX, torsoY, 0.0F}, torsoWidth, torsoHeight, torsoDepth, shirtColor);
    DrawCubeWires({leanedX, torsoY, 0.0F}, torsoWidth, torsoHeight, torsoDepth, DARKBLUE);

    // Barriga proporcional ao peso
    if (weightState != WeightState::Normal)
    {
        const float bellyRadius = (weightState == WeightState::Overweight ? 0.28F : 0.42F) * scale;
        DrawSphere({leanedX, torsoY - 0.08F * scale, 0.16F * girthScale}, bellyRadius, shirtColor);
    }

    // 2. CABEÇA
    const float headY      = torsoY + (crouching ? 0.28F : 0.52F) * scale * stretchY;
    const float headZ      = (crouching ? 0.18F : 0.0F);
    const float headRadius = 0.22F * scale;
    DrawSphere({leanedX, headY, headZ}, headRadius, skinColor);
    DrawCylinder({leanedX, headY + 0.04F * scale, headZ}, headRadius * 1.05F, headRadius * 1.05F, 0.06F * scale, 12, RED);
    DrawCube({leanedX, headY, headZ + headRadius * 0.95F}, 0.26F * scale, 0.08F * scale, 0.06F * scale, Color{20, 20, 25, 255});

    // 3. BRAÇOS
    const float armY   = torsoY + (crouching ? 0.05F : 0.15F) * scale;
    const float armSpan = (torsoWidth * 0.5F) + 0.08F;

    if (jumpHeight > 0.0F)
    {
        // Braços abertos no ar
        DrawCube({leanedX - armSpan - 0.1F, armY + 0.15F, 0.0F}, 0.12F, 0.35F, 0.12F, skinColor);
        DrawCube({leanedX + armSpan + 0.1F, armY + 0.15F, 0.0F}, 0.12F, 0.35F, 0.12F, skinColor);
    }
    else if (crouching)
    {
        DrawCube({leanedX - armSpan, armY - 0.1F, 0.1F}, 0.12F, 0.25F, 0.12F, skinColor);
        DrawCube({leanedX + armSpan, armY - 0.1F, 0.1F}, 0.12F, 0.25F, 0.12F, skinColor);
    }
    else
    {
        const float leftArmZ  = -runCycle * 0.22F;
        const float rightArmZ =  runCycle * 0.22F;
        DrawCube({leanedX - armSpan, armY,         leftArmZ},         0.12F, 0.40F, 0.12F, shirtColor);
        DrawCube({leanedX - armSpan, armY - 0.22F, leftArmZ + 0.05F}, 0.10F, 0.18F, 0.10F, skinColor);
        DrawCube({leanedX + armSpan, armY,          rightArmZ},         0.12F, 0.40F, 0.12F, shirtColor);
        DrawCube({leanedX + armSpan, armY - 0.22F, rightArmZ + 0.05F}, 0.10F, 0.18F, 0.10F, skinColor);
    }

    // 4. PERNAS
    const float legSpacing = 0.14F * girthScale;
    const float legBaseY   = jumpHeight + (crouching ? 0.12F : 0.30F) * scale;

    if (jumpHeight > 0.0F)
    {
        DrawCube({laneX - legSpacing, legBaseY + 0.1F, -0.15F}, 0.14F, 0.35F, 0.14F, shortsColor);
        DrawCube({laneX + legSpacing, legBaseY + 0.1F, -0.15F}, 0.14F, 0.35F, 0.14F, shortsColor);
        DrawCube({laneX - legSpacing, legBaseY - 0.05F, -0.25F}, 0.15F, 0.10F, 0.24F, RAYWHITE);
        DrawCube({laneX + legSpacing, legBaseY - 0.05F, -0.25F}, 0.15F, 0.10F, 0.24F, RAYWHITE);
    }
    else if (crouching)
    {
        DrawCube({laneX - legSpacing, legBaseY, 0.0F}, 0.14F, 0.20F, 0.30F, shortsColor);
        DrawCube({laneX + legSpacing, legBaseY, 0.0F}, 0.14F, 0.20F, 0.30F, shortsColor);
        DrawCube({laneX - legSpacing, 0.06F, -0.10F}, 0.16F, 0.10F, 0.24F, RAYWHITE);
        DrawCube({laneX + legSpacing, 0.06F, -0.10F}, 0.16F, 0.10F, 0.24F, RAYWHITE);
    }
    else
    {
        const float leftLegZ  =  runCycle * 0.25F;
        const float rightLegZ = -runCycle * 0.25F;
        DrawCube({laneX - legSpacing, legBaseY, leftLegZ},         0.14F, 0.55F, 0.14F, shortsColor);
        DrawCube({laneX - legSpacing, 0.07F,    leftLegZ + 0.05F}, 0.16F, 0.12F, 0.26F, RAYWHITE);
        DrawCube({laneX + legSpacing, legBaseY, rightLegZ},         0.14F, 0.55F, 0.14F, shortsColor);
        DrawCube({laneX + legSpacing, 0.07F,    rightLegZ + 0.05F}, 0.16F, 0.12F, 0.26F, RAYWHITE);
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

    const float height    = (crouching ? Settings::PLAYER_HEIGHT * 0.35F : Settings::PLAYER_HEIGHT) * scale;
    const float width     = Settings::PLAYER_WIDTH * girthScale;
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
