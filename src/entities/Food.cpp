#include "Food.h"

#include <cmath>
#include <rlgl.h>

#include "core/Settings.h"

void Food::SetPosition(Vector3 newPosition)
{
    position = newPosition;
}

Vector3 Food::GetPosition() const
{
    return position;
}

bool Food::IsActive() const
{
    return active;
}

void Food::SetActive(bool isActive)
{
    active = isActive;
    if (isActive)
    {
        rotation = static_cast<float>(GetRandomValue(0, 360));
    }
}

BoundingBox Food::GetBoundingBox() const
{
    const float radius = Settings::FOOD_RADIUS;
    return {
        {position.x - radius, position.y - radius, position.z - radius},
        {position.x + radius, position.y + radius, position.z + radius}
    };
}

void Food::Update(float deltaTime)
{
    if (active)
    {
        rotation += 110.0F * deltaTime;
        if (rotation >= 360.0F)
        {
            rotation -= 360.0F;
        }
    }
}

void Food::Draw() const
{
    if (!active)
    {
        return;
    }

    // Leve flutuação senoidal arcade
    const float bob       = 0.06F * std::sin(rotation * 0.05F);
    const Vector3 drawPos = {position.x, position.y + bob, position.z};

    // 1. Anel / Aura brilhante no chão abaixo do item (com pulsação)
    const bool isGood = (GetType() == FoodType::Good);
    const float pulse = 0.92F + 0.08F * std::sin(rotation * 0.08F);
    const float ringRadius = 0.54F * pulse;

    if (isGood)
    {
        // Verde vibrante com anel externo reluzente
        const Color auraColor = Color{46, 204, 113, 175};
        DrawCylinder({drawPos.x, 0.018F, drawPos.z}, ringRadius, ringRadius, 0.015F, 16, auraColor);
        DrawCylinderWires({drawPos.x, 0.018F, drawPos.z}, ringRadius, ringRadius, 0.015F, 16, Fade(GREEN, 0.8F));
    }
    else
    {
        // Vermelho alerta para comidas calóricas
        const Color auraColor = Color{231, 76, 60, 175};
        DrawCylinder({drawPos.x, 0.018F, drawPos.z}, ringRadius, ringRadius, 0.015F, 16, auraColor);
        DrawCylinderWires({drawPos.x, 0.018F, drawPos.z}, ringRadius, ringRadius, 0.015F, 16, Fade(RED, 0.8F));
    }

    // 2. Modelo 3D rotacionado do alimento
    rlPushMatrix();
    rlTranslatef(drawPos.x, drawPos.y, drawPos.z);
    rlRotatef(rotation, 0.0F, 1.0F, 0.0F);
    DrawModel({0.0F, 0.0F, 0.0F}, rotation);
    rlPopMatrix();
}

FoodAltitude Food::GetAltitude() const
{
    return altitude;
}

void Food::SetAltitude(FoodAltitude alt)
{
    altitude = alt;
}

// ==========================================
// MÉTODOS BASE GOOD FOOD / BAD FOOD
// ==========================================
void GoodFood::ApplyTo(Player& player) const
{
    player.OnCollideGoodFood(GetWeightValue());
}

Color GoodFood::GetColor() const
{
    return Color{46, 204, 113, 255};
}

void BadFood::ApplyTo(Player& player) const
{
    player.OnCollideBadFood(GetWeightValue());
}

Color BadFood::GetColor() const
{
    return Color{231, 76, 60, 255};
}

// ==========================================
// MODELOS DETALHADOS DE COMIDAS SAUDÁVEIS
// ==========================================
const char* Apple::GetDisplayName() const { return "Maca Verde"; }
float Apple::GetWeightValue() const { return 8.0F; }
int Apple::GetScoreBonus() const { return 100; }
void Apple::DrawModel(Vector3, float) const
{
    DrawSphere({0.0F, 0.02F, 0.0F}, 0.38F, Color{106, 190, 48, 255});
    DrawSphere({0.0F, -0.06F, 0.0F}, 0.34F, Color{90, 175, 40, 255});
    DrawSphereWires({0.0F, 0.0F, 0.0F}, 0.38F, 8, 8, Color{60, 135, 25, 255});
    DrawCylinder({0.0F, 0.30F, 0.0F}, 0.035F, 0.035F, 0.18F, 6, Color{101, 67, 33, 255});
    DrawCube({0.10F, 0.38F, 0.0F}, 0.16F, 0.03F, 0.08F, Color{40, 180, 40, 255});
    DrawCubeWires({0.10F, 0.38F, 0.0F}, 0.16F, 0.03F, 0.08F, Color{25, 120, 25, 255});
    DrawSphere({0.12F, 0.18F, 0.22F}, 0.06F, Fade(RAYWHITE, 0.7F));
}

const char* Carrot::GetDisplayName() const { return "Cenoura Fresca"; }
float Carrot::GetWeightValue() const { return 10.0F; }
int Carrot::GetScoreBonus() const { return 120; }
void Carrot::DrawModel(Vector3, float) const
{
    DrawCylinder({0.0F, 0.0F, 0.0F}, 0.24F, 0.05F, 0.65F, 12, Color{255, 130, 20, 255});
    DrawCylinderWires({0.0F, 0.0F, 0.0F}, 0.24F, 0.05F, 0.65F, 12, Color{210, 95, 15, 255});
    DrawCylinder({0.0F,  0.10F, 0.0F}, 0.21F, 0.21F, 0.02F, 12, Color{235, 110, 10, 255});
    DrawCylinder({0.0F, -0.10F, 0.0F}, 0.14F, 0.14F, 0.02F, 12, Color{235, 110, 10, 255});
    DrawCylinder({0.0F,  0.38F, 0.0F}, 0.15F, 0.05F, 0.22F, 8, Color{46, 204, 113, 255});
    DrawSphere({-0.06F, 0.46F, 0.02F}, 0.11F, Color{39, 174, 96, 255});
    DrawSphere({ 0.06F, 0.46F,-0.02F}, 0.11F, Color{46, 204, 113, 255});
    DrawSphere({ 0.0F,  0.52F, 0.0F},  0.09F, Color{88, 214, 141, 255});
}

const char* GrilledSteak::GetDisplayName() const { return "Carne Grelhada"; }
float GrilledSteak::GetWeightValue() const { return 14.0F; }
int GrilledSteak::GetScoreBonus() const { return 160; }
void GrilledSteak::DrawModel(Vector3, float) const
{
    DrawCube({0.0F, 0.0F, 0.0F}, 0.68F, 0.18F, 0.52F, Color{130, 65, 25, 255});
    DrawCubeWires({0.0F, 0.0F, 0.0F}, 0.68F, 0.18F, 0.52F, Color{75, 35, 12, 255});
    for (float off = -0.20F; off <= 0.20F; off += 0.20F)
    {
        DrawCube({off, 0.10F, 0.0F}, 0.045F, 0.02F, 0.48F, Color{40, 18, 8, 255});
    }
    DrawCube({0.0F, 0.12F, 0.0F}, 0.14F, 0.06F, 0.14F, Color{255, 240, 140, 255});
    DrawSphere({0.02F, 0.14F, -0.02F}, 0.03F, Color{34, 139, 34, 255});
}

// ==========================================
// MODELOS DETALHADOS DE COMIDAS RUINS
// ==========================================
const char* Burger::GetDisplayName() const { return "Hamburguer Triplo"; }
float Burger::GetWeightValue() const { return 18.0F; }
int Burger::GetScoreBonus() const { return 0; }
void Burger::DrawModel(Vector3, float) const
{
    DrawCylinder({0.0F, -0.16F, 0.0F}, 0.42F, 0.42F, 0.10F, 16, Color{225, 175, 100, 255});
    DrawCylinder({0.0F, -0.06F, 0.0F}, 0.44F, 0.44F, 0.11F, 16, Color{80,  38,  18, 255});
    DrawCube({0.0F, 0.01F, 0.0F}, 0.54F, 0.03F, 0.54F, Color{255, 195, 0, 255});
    DrawCylinder({0.0F,  0.05F, 0.0F}, 0.42F, 0.42F, 0.04F, 16, Color{220, 40, 30, 255});
    DrawCube({-0.15F, 0.08F,  0.15F}, 0.22F, 0.03F, 0.22F, Color{46, 204, 113, 255});
    DrawCube({ 0.15F, 0.08F, -0.15F}, 0.22F, 0.03F, 0.22F, Color{46, 204, 113, 255});
    DrawSphere({0.0F, 0.14F, 0.0F}, 0.41F, Color{225, 175, 100, 255});
    DrawSphereWires({0.0F, 0.14F, 0.0F}, 0.41F, 10, 10, Color{195, 145, 75, 255});
    const Vector3 sesameSeeds[] = {
        {-0.14F, 0.32F, 0.10F}, {0.14F, 0.32F, -0.08F},
        { 0.00F, 0.34F, 0.14F}, {-0.08F, 0.33F, -0.12F},
        { 0.12F, 0.31F, 0.12F}, { 0.00F, 0.36F,  0.00F}
    };
    for (const Vector3& seed : sesameSeeds)
    {
        DrawSphere(seed, 0.025F, Color{255, 250, 230, 255});
    }
}

const char* Fries::GetDisplayName() const { return "Batata Frita"; }
float Fries::GetWeightValue() const { return 14.0F; }
int Fries::GetScoreBonus() const { return 0; }
void Fries::DrawModel(Vector3, float) const
{
    DrawCube({0.0F, -0.06F, 0.0F}, 0.46F, 0.44F, 0.30F, Color{220, 25, 25, 255});
    DrawCubeWires({0.0F, -0.06F, 0.0F}, 0.46F, 0.44F, 0.30F, Color{160, 15, 15, 255});
    DrawCube({0.0F, -0.06F, 0.16F}, 0.20F, 0.14F, 0.02F, GOLD);
    const struct FryStick { float x; float y; float z; float h; } sticks[] = {
        {-0.15F, 0.22F,  0.05F, 0.32F},
        {-0.08F, 0.26F, -0.04F, 0.40F},
        {-0.02F, 0.29F,  0.03F, 0.44F},
        { 0.06F, 0.25F, -0.02F, 0.38F},
        { 0.14F, 0.21F,  0.04F, 0.30F},
        {-0.10F, 0.18F,  0.08F, 0.24F},
        { 0.08F, 0.19F,  0.08F, 0.26F}
    };
    for (const auto& stick : sticks)
    {
        DrawCube({stick.x, stick.y, stick.z}, 0.07F, stick.h, 0.07F, Color{255, 205, 40, 255});
        DrawCubeWires({stick.x, stick.y, stick.z}, 0.07F, stick.h, 0.07F, Color{210, 155, 20, 255});
    }
}

const char* SodaCan::GetDisplayName() const { return "Refrigerante Cola"; }
float SodaCan::GetWeightValue() const { return 12.0F; }
int SodaCan::GetScoreBonus() const { return 0; }
void SodaCan::DrawModel(Vector3, float) const
{
    DrawCylinder({0.0F,  0.0F, 0.0F}, 0.27F, 0.27F, 0.62F, 16, Color{210, 20, 20, 255});
    DrawCylinder({0.0F,  0.02F, 0.0F}, 0.272F, 0.272F, 0.14F, 16, RAYWHITE);
    DrawCylinder({0.0F,  0.32F, 0.0F}, 0.26F, 0.27F, 0.04F, 16, LIGHTGRAY);
    DrawCylinder({0.0F, -0.32F, 0.0F}, 0.27F, 0.26F, 0.04F, 16, LIGHTGRAY);
    DrawCylinder({0.08F, 0.44F, 0.0F}, 0.035F, 0.035F, 0.26F, 8, RAYWHITE);
    DrawCylinder({0.08F, 0.48F, 0.0F}, 0.037F, 0.037F, 0.05F, 8, RED);
}

const char* Donut::GetDisplayName() const { return "Donut Acucarado"; }
float Donut::GetWeightValue() const { return 16.0F; }
int Donut::GetScoreBonus() const { return 0; }
void Donut::DrawModel(Vector3, float) const
{
    DrawCylinder({0.0F, 0.0F, 0.0F}, 0.40F, 0.40F, 0.20F, 18, Color{225, 175, 115, 255});
    DrawCylinderWires({0.0F, 0.0F, 0.0F}, 0.40F, 0.40F, 0.20F, 18, Color{185, 135, 80, 255});
    DrawCylinder({0.0F, 0.08F, 0.0F}, 0.38F, 0.38F, 0.07F, 18, Color{255, 105, 180, 255});
    const struct Sprinkle { float x; float z; Color c; } sprinkles[] = {
        {-0.16F, -0.12F, SKYBLUE}, {0.14F, 0.14F, GOLD},
        {-0.12F,  0.16F, LIME},    {0.18F,-0.09F, RAYWHITE},
        { 0.00F, -0.22F, Color{155, 89, 182, 255}},
        {-0.22F,  0.04F, YELLOW},  {0.20F, 0.08F, Color{52, 152, 219, 255}},
        { 0.06F,  0.22F, Color{231, 76, 60, 255}}
    };
    for (const auto& sp : sprinkles)
    {
        DrawCube({sp.x, 0.12F, sp.z}, 0.06F, 0.025F, 0.03F, sp.c);
    }
}
