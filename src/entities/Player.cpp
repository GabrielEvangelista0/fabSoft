#include "Player.h"

#include <algorithm>
#include <cmath>
#include <initializer_list>
#include <rlgl.h>

#include "core/Settings.h"

Player::Player()
{
    Reset();
}

void Player::Reset()
{
    targetLane    = 0;
    laneX         = 0.0F;
    prevLaneX     = 0.0F;
    jumpHeight    = 0.0F;
    verticalSpeed = 0.0F;
    runPhase      = 0.0F;
    runSpeed      = Settings::WORLD_SPEED;
    weight        = 20.0F;
    crouching     = false;
    weightState   = WeightState::Normal;
    leanAngle     = 0.0F;
    stretchTimer  = 0.0F;
}

void Player::Update(float deltaTime, float worldSpeed)
{
    runSpeed = std::max(0.0F, worldSpeed);

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
        stretchTimer  = 0.18F; // Efeito stretch no impulso do salto
    }

    crouching = IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S);

    prevLaneX = laneX;
    // Em visão com câmera virada para +Z, targetX positivo corresponde à esquerda da tela
    const float targetX = static_cast<float>(-targetLane) * Settings::LANE_WIDTH;
    const float laneChangeAmount = std::min(1.0F, Settings::PLAYER_LANE_CHANGE_SPEED * deltaTime);
    laneX += (targetX - laneX) * laneChangeAmount;

    // Inclinação lateral (banking) nas trocas de faixa
    const float lateralVel = (laneX - prevLaneX) / std::max(0.0001F, deltaTime);
    const float targetLean = std::clamp(-lateralVel * 8.0F, -14.0F, 14.0F);
    leanAngle += (targetLean - leanAngle) * std::min(1.0F, 12.0F * deltaTime);

    if (jumpHeight > 0.0F || verticalSpeed > 0.0F)
    {
        jumpHeight    += verticalSpeed * deltaTime;
        verticalSpeed -= Settings::GRAVITY * deltaTime;

        if (jumpHeight <= 0.0F)
        {
            jumpHeight    = 0.0F;
            verticalSpeed = 0.0F;
            stretchTimer  = 0.12F; // Efeito squash na aterrissagem
        }
    }

    if (stretchTimer > 0.0F)
    {
        stretchTimer -= deltaTime;
    }

    runPhase = std::fmod(
        runPhase + deltaTime * (7.5F + runSpeed * 0.5F),
        6.28318530718F);
}

void Player::Draw() const
{
    const float scale = (weightState == WeightState::Normal) ? 1.0F
        : (weightState == WeightState::Overweight) ? 1.25F : 1.55F;
    const float girthScale = (weightState == WeightState::Normal) ? 1.0F
        : (weightState == WeightState::Overweight) ? 1.3F : 1.7F;
    const bool airborne = jumpHeight > 0.01F;
    const float runCycle = std::sin(runPhase);
    const float bob = (!airborne && !crouching) ? 0.035F * std::sin(runPhase * 2.0F) : 0.0F;
    const float hipPivotY = crouching ? 0.68F : 0.74F;

    float stretchX = 1.0F;
    float stretchY = 1.0F;
    if (stretchTimer > 0.0F)
    {
        const bool stretching = verticalSpeed > 0.0F || jumpHeight > 0.1F;
        const float duration = stretching ? 0.18F : 0.12F;
        const float amount = std::clamp(stretchTimer / duration, 0.0F, 1.0F);
        stretchX = stretching ? 1.0F - amount * 0.08F : 1.0F + amount * 0.10F;
        stretchY = stretching ? 1.0F + amount * 0.12F : 1.0F - amount * 0.08F;
    }

    const float airAmount = std::clamp(jumpHeight / 1.8F, 0.0F, 1.0F);
    const float shadowRadius = 0.42F * girthScale * (1.0F - 0.42F * airAmount);
    const float shadowAlpha = 0.52F * (1.0F - 0.62F * airAmount);
    DrawCylinder(
        {laneX, 0.015F, 0.0F},
        shadowRadius,
        shadowRadius,
        0.018F,
        20,
        Fade(Color{18, 22, 30, 255}, shadowAlpha));

    Color shirtColor{};
    Color shirtHighlight{};
    Color shirtShadow{};
    if (weightState == WeightState::Normal)
    {
        shirtColor = Color{28, 112, 205, 255};
        shirtHighlight = Color{90, 180, 245, 255};
        shirtShadow = Color{18, 65, 130, 255};
    }
    else if (weightState == WeightState::Overweight)
    {
        shirtColor = Color{220, 145, 24, 255};
        shirtHighlight = Color{255, 198, 78, 255};
        shirtShadow = Color{145, 84, 12, 255};
    }
    else
    {
        shirtColor = Color{195, 38, 42, 255};
        shirtHighlight = Color{245, 100, 92, 255};
        shirtShadow = Color{115, 20, 28, 255};
    }

    const Color skinColor{239, 190, 153, 255};
    const Color hairColor{60, 42, 32, 255};
    const Color shortsColor{32, 42, 62, 255};
    const Color shoeColor{235, 244, 250, 255};
    const Color soleColor{34, 42, 54, 255};
    const float forwardLean = crouching ? 22.0F
        : 7.0F + std::clamp((runSpeed - 9.0F) * 0.18F, 0.0F, 4.0F);

    rlPushMatrix();
    rlTranslatef(laneX, jumpHeight + bob + hipPivotY * scale * stretchY, 0.0F);
    rlRotatef(leanAngle, 0.0F, 0.0F, 1.0F);
    rlRotatef(forwardLean, 1.0F, 0.0F, 0.0F);
    rlScalef(girthScale * stretchX, scale * stretchY, girthScale);
    rlTranslatef(0.0F, -hipPivotY, 0.0F);

    const float torsoY = crouching ? 0.28F : 1.04F;
    const float shoulderY = crouching ? 0.41F : 1.34F;
    const float headY = crouching ? 0.43F : 1.67F;
    const float headRadius = crouching ? 0.17F : 0.22F;
    const float torsoHeight = crouching ? 0.32F : 0.56F;
    const float upperTorsoHeight = crouching ? 0.25F : 0.34F;
    const float upperTorsoOffset = crouching ? 0.12F : 0.23F;

    DrawCube({0.0F, torsoY, 0.0F}, 0.58F, torsoHeight, 0.40F, shirtColor);
    DrawCube({0.0F, torsoY + upperTorsoOffset, 0.0F}, 0.74F, upperTorsoHeight, 0.42F, shirtColor);

    // Jersey yoke, seams and side panels create readable, light-facing planes.
    const float yokeY = torsoY + (crouching ? 0.24F : 0.39F);
    DrawCube({0.0F, yokeY, -0.218F}, 0.59F, 0.045F, 0.025F, shirtHighlight);
    DrawCube({0.0F, torsoY + 0.02F, -0.207F}, 0.055F, torsoHeight * 0.75F, 0.025F, shirtHighlight);
    DrawCube({-0.263F, torsoY + 0.01F, -0.205F}, 0.045F, torsoHeight * 0.78F, 0.025F, shirtShadow);
    DrawCube({0.263F, torsoY + 0.01F, -0.205F}, 0.045F, torsoHeight * 0.78F, 0.025F, shirtShadow);
    DrawCube({0.0F, yokeY + 0.01F, 0.0F}, 0.22F, 0.08F, 0.24F, shirtShadow);
    DrawCube({0.0F, yokeY + 0.01F, -0.13F}, 0.17F, 0.07F, 0.045F, shirtHighlight);

    const float shortsY = (crouching ? 0.36F : 0.74F);
    const float shortsHeight = crouching ? 0.23F : 0.30F;
    DrawCube({0.0F, shortsY, 0.0F}, 0.52F, shortsHeight, 0.36F, shortsColor);
    DrawCube({0.0F, shortsY + shortsHeight * 0.5F, 0.0F}, 0.53F, 0.045F, 0.37F, Color{54, 68, 91, 255});
    DrawCube({-0.235F, shortsY, -0.187F}, 0.035F, shortsHeight * 0.75F, 0.025F, Color{66, 177, 225, 255});
    DrawCube({0.235F, shortsY, -0.187F}, 0.035F, shortsHeight * 0.75F, 0.025F, Color{66, 177, 225, 255});

    const float neckY = headY - (crouching ? 0.13F : 0.27F);
    DrawCube({0.0F, neckY, 0.0F}, 0.17F, crouching ? 0.18F : 0.20F, 0.18F, skinColor);

    rlPushMatrix();
    rlTranslatef(0.0F, headY, 0.0F);
    rlRotatef(runCycle * 2.0F, 1.0F, 0.0F, 0.0F);
    DrawSphere({0.0F, 0.0F, 0.0F}, headRadius, skinColor);
    DrawSphere({0.0F, -0.01F, -0.075F}, headRadius * 0.82F, hairColor);
    DrawSphere({-headRadius * 0.93F, -0.015F, 0.0F}, headRadius * 0.25F, skinColor);
    DrawSphere({headRadius * 0.93F, -0.015F, 0.0F}, headRadius * 0.25F, skinColor);
    DrawCylinder({0.0F, 0.045F, 0.0F}, headRadius * 1.04F, headRadius * 1.04F, 0.065F, 14, RED);
    const float headScale = headRadius / 0.22F;
    DrawCube({0.0F, 0.085F * headScale, -0.19F * headScale},
             0.12F * headScale, 0.075F * headScale, 0.12F * headScale, Color{190, 28, 35, 255});

    for (const float side : {-1.0F, 1.0F})
    {
        rlPushMatrix();
        rlTranslatef(side * 0.13F, 0.015F, -0.13F);
        rlRotatef(-runCycle * 10.0F + side * 8.0F, 1.0F, 0.0F, 0.0F);
        rlRotatef(side * 10.0F, 0.0F, 1.0F, 0.0F);
        DrawCube({side * 0.045F * headScale, -0.015F * headScale, -0.14F * headScale},
                 0.075F * headScale, 0.075F * headScale, 0.30F * headScale, RED);
        rlPopMatrix();
    }
    rlPopMatrix();

    for (const float side : {-1.0F, 1.0F})
    {
        const float armSwing = airborne ? 0.0F : (crouching ? side * 18.0F : side * runCycle * 34.0F);
        const float shoulderRoll = airborne ? side * 25.0F : 0.0F;
        const float armPivotY = shoulderY - 0.02F;

        rlPushMatrix();
        rlTranslatef(side * 0.36F, armPivotY, 0.0F);
        rlRotatef(shoulderRoll, 0.0F, 0.0F, 1.0F);
        rlRotatef(armSwing + (airborne ? -12.0F : 0.0F), 1.0F, 0.0F, 0.0F);
        DrawSphere({0.0F, -0.015F, 0.0F}, 0.12F, shirtColor);
        DrawCube({0.0F, -0.17F, 0.0F}, 0.19F, 0.34F, 0.19F, shirtColor);
        DrawCube({0.0F, -0.33F, -0.005F}, 0.17F, 0.045F, 0.18F, shirtHighlight);

        rlTranslatef(0.0F, -0.33F, 0.0F);
        const float elbowBend = crouching ? -48.0F : (airborne ? -38.0F : -armSwing * 0.35F);
        rlRotatef(elbowBend, 1.0F, 0.0F, 0.0F);
        DrawCube({0.0F, -0.14F, 0.0F}, 0.13F, 0.28F, 0.14F, skinColor);
        rlTranslatef(0.0F, -0.28F, 0.0F);
        DrawSphere({0.0F, -0.045F, 0.0F}, 0.085F, skinColor);
        rlPopMatrix();
    }

    for (const float side : {-1.0F, 1.0F})
    {
        const float legSwing = airborne ? -side * runCycle * 9.0F
            : (crouching ? side * 8.0F : -side * runCycle * 32.0F);
        const float kneeBend = airborne ? 42.0F : (crouching ? 48.0F : std::max(0.0F, legSwing) * 0.45F);
        const float upperLegHeight = crouching ? 0.23F : 0.32F;
        const float calfHeight = crouching ? 0.22F : 0.28F;
        const float legPivotY = crouching ? 0.43F : 0.68F;

        rlPushMatrix();
        rlTranslatef(side * 0.15F, legPivotY, 0.0F);
        rlRotatef(legSwing, 1.0F, 0.0F, 0.0F);
        DrawCube({0.0F, -upperLegHeight * 0.5F, 0.0F}, 0.19F, upperLegHeight, 0.20F, shortsColor);
        DrawCube({side * 0.055F, -upperLegHeight * 0.5F, -0.105F},
                 0.025F, upperLegHeight * 0.72F, 0.025F, Color{66, 177, 225, 255});

        rlTranslatef(0.0F, -upperLegHeight, 0.0F);
        rlRotatef(kneeBend, 1.0F, 0.0F, 0.0F);
        DrawCube({0.0F, -calfHeight * 0.5F, 0.0F}, 0.14F, calfHeight, 0.15F, skinColor);
        DrawCube({0.0F, -calfHeight * 0.88F, 0.0F}, 0.15F, 0.05F, 0.16F, Color{38, 53, 78, 255});

        rlTranslatef(0.0F, -calfHeight, 0.0F);
        DrawCube({0.0F, -0.045F, 0.055F}, 0.25F, 0.13F, 0.36F, shoeColor);
        DrawCube({0.0F, -0.105F, 0.055F}, 0.26F, 0.035F, 0.37F, soleColor);
        DrawCube({0.0F, -0.005F, -0.105F}, 0.16F, 0.08F, 0.05F, Color{255, 120, 45, 255});
        rlPopMatrix();
    }

    rlPopMatrix();
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
