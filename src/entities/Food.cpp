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
        rotation += 90.0F * deltaTime;
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

    rlPushMatrix();
    rlTranslatef(position.x, position.y, position.z);
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
    return LIME;
}

void BadFood::ApplyTo(Player& player) const
{
    player.OnCollideBadFood(GetWeightValue());
}

Color BadFood::GetColor() const
{
    return RED;
}

// ==========================================
// MODELOS ESPECÍFICOS DE COMIDAS SAUDÁVEIS
// ==========================================
const char* Apple::GetDisplayName() const { return "Maca Verde"; }
float Apple::GetWeightValue() const { return 8.0F; }
int Apple::GetScoreBonus() const { return 100; }
void Apple::DrawModel(Vector3, float) const
{
    // Maçã verde brilhante com cabinho e folha
    DrawSphere({0.0F, 0.0F, 0.0F}, 0.38F, Color{106, 190, 48, 255});
    DrawSphereWires({0.0F, 0.0F, 0.0F}, 0.38F, 8, 8, Color{70, 140, 30, 255});
    // Cabinho marrom
    DrawCylinder({0.0F, 0.32F, 0.0F}, 0.03F, 0.03F, 0.16F, 6, Color{101, 67, 33, 255});
    // Folhinha verde
    DrawCube({0.09F, 0.38F, 0.0F}, 0.12F, 0.03F, 0.07F, Color{40, 160, 40, 255});
}

const char* Carrot::GetDisplayName() const { return "Cenoura Fresca"; }
float Carrot::GetWeightValue() const { return 10.0F; }
int Carrot::GetScoreBonus() const { return 120; }
void Carrot::DrawModel(Vector3, float) const
{
    // Corpo cônico laranja da cenoura
    DrawCylinder({0.0F, 0.0F, 0.0F}, 0.22F, 0.05F, 0.60F, 10, Color{255, 130, 20, 255});
    DrawCylinderWires({0.0F, 0.0F, 0.0F}, 0.22F, 0.05F, 0.60F, 10, Color{200, 90, 10, 255});
    // Ramalhete de folhas verdes no topo
    DrawCylinder({0.0F, 0.36F, 0.0F}, 0.14F, 0.04F, 0.22F, 8, Color{40, 180, 50, 255});
    DrawSphere({0.0F, 0.44F, 0.0F}, 0.13F, Color{30, 160, 40, 255});
}

const char* GrilledSteak::GetDisplayName() const { return "Carne Grelhada"; }
float GrilledSteak::GetWeightValue() const { return 14.0F; }
int GrilledSteak::GetScoreBonus() const { return 160; }
void GrilledSteak::DrawModel(Vector3, float) const
{
    // Bife grelhado suculento
    DrawCube({0.0F, 0.0F, 0.0F}, 0.65F, 0.16F, 0.50F, Color{130, 65, 25, 255});
    DrawCubeWires({0.0F, 0.0F, 0.0F}, 0.65F, 0.16F, 0.50F, Color{80, 40, 15, 255});
    // Marcas de grelha tostadas
    DrawCube({-0.16F, 0.09F, 0.0F}, 0.04F, 0.02F, 0.46F, Color{40, 20, 10, 255});
    DrawCube({0.0F, 0.09F, 0.0F}, 0.04F, 0.02F, 0.46F, Color{40, 20, 10, 255});
    DrawCube({0.16F, 0.09F, 0.0F}, 0.04F, 0.02F, 0.46F, Color{40, 20, 10, 255});
}

// ==========================================
// MODELOS ESPECÍFICOS DE COMIDAS RUINS
// ==========================================
const char* Burger::GetDisplayName() const { return "Hamburguer Triplo"; }
float Burger::GetWeightValue() const { return 18.0F; }
int Burger::GetScoreBonus() const { return 0; }
void Burger::DrawModel(Vector3, float) const
{
    // Pão inferior
    DrawCylinder({0.0F, -0.16F, 0.0F}, 0.40F, 0.40F, 0.10F, 14, Color{225, 175, 100, 255});
    // Hambúrguer de carne
    DrawCylinder({0.0F, -0.05F, 0.0F}, 0.43F, 0.43F, 0.10F, 14, Color{85, 40, 20, 255});
    // Queijo cheddar derretido
    DrawCube({0.0F, 0.02F, 0.0F}, 0.52F, 0.03F, 0.52F, Color{255, 205, 0, 255});
    // Rodela de tomate / alface
    DrawCylinder({0.0F, 0.06F, 0.0F}, 0.41F, 0.41F, 0.04F, 14, Color{220, 40, 30, 255});
    // Pão superior arredondado
    DrawSphere({0.0F, 0.14F, 0.0F}, 0.39F, Color{225, 175, 100, 255});
    DrawSphereWires({0.0F, 0.14F, 0.0F}, 0.39F, 8, 8, Color{190, 140, 70, 255});
}

const char* Fries::GetDisplayName() const { return "Batata Frita"; }
float Fries::GetWeightValue() const { return 14.0F; }
int Fries::GetScoreBonus() const { return 0; }
void Fries::DrawModel(Vector3, float) const
{
    // Caixinha vermelha
    DrawCube({0.0F, -0.06F, 0.0F}, 0.44F, 0.42F, 0.28F, Color{220, 25, 25, 255});
    DrawCubeWires({0.0F, -0.06F, 0.0F}, 0.44F, 0.42F, 0.28F, Color{150, 15, 15, 255});
    // Palitos crocantes de batata saindo do pacote
    DrawCube({-0.12F, 0.22F, 0.04F}, 0.07F, 0.30F, 0.07F, GOLD);
    DrawCube({-0.04F, 0.27F, -0.03F}, 0.07F, 0.38F, 0.07F, GOLD);
    DrawCube({0.05F, 0.24F, 0.03F}, 0.07F, 0.34F, 0.07F, GOLD);
    DrawCube({0.13F, 0.20F, -0.04F}, 0.07F, 0.28F, 0.07F, GOLD);
}

const char* SodaCan::GetDisplayName() const { return "Refrigerante Cola"; }
float SodaCan::GetWeightValue() const { return 12.0F; }
int SodaCan::GetScoreBonus() const { return 0; }
void SodaCan::DrawModel(Vector3, float) const
{
    // Lata de refrigerante vermelha
    DrawCylinder({0.0F, 0.0F, 0.0F}, 0.26F, 0.26F, 0.58F, 14, Color{210, 20, 20, 255});
    // Borda superior e inferior prateadas
    DrawCylinder({0.0F, 0.30F, 0.0F}, 0.26F, 0.26F, 0.04F, 14, LIGHTGRAY);
    DrawCylinder({0.0F, -0.30F, 0.0F}, 0.26F, 0.26F, 0.04F, 14, LIGHTGRAY);
    // Canudo listrado saindo
    DrawCylinder({0.08F, 0.42F, 0.0F}, 0.03F, 0.03F, 0.24F, 8, RAYWHITE);
}

const char* Donut::GetDisplayName() const { return "Donut Acucarado"; }
float Donut::GetWeightValue() const { return 16.0F; }
int Donut::GetScoreBonus() const { return 0; }
void Donut::DrawModel(Vector3, float) const
{
    // Massa frita dourada
    DrawCylinder({0.0F, 0.0F, 0.0F}, 0.38F, 0.38F, 0.18F, 16, Color{225, 175, 115, 255});
    // Cobertura rosa brilhante
    DrawCylinder({0.0F, 0.07F, 0.0F}, 0.36F, 0.36F, 0.06F, 16, Color{255, 105, 180, 255});
    // Confeitos coloridos
    DrawCube({-0.14F, 0.11F, -0.10F}, 0.06F, 0.02F, 0.03F, BLUE);
    DrawCube({0.12F, 0.11F, 0.12F}, 0.06F, 0.02F, 0.03F, YELLOW);
    DrawCube({-0.10F, 0.11F, 0.14F}, 0.06F, 0.02F, 0.03F, LIME);
    DrawCube({0.15F, 0.11F, -0.08F}, 0.06F, 0.02F, 0.03F, WHITE);
}
