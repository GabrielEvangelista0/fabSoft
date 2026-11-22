#pragma once

#include "entities/Player.h"
#include "raylib.h"

enum class FoodType
{
    Good,
    Bad
};

enum class FoodAltitude
{
    Ground,     // No chão: pular para desviar se ruim, ou passar por cima
    Suspended   // No ar: agachar para passar por baixo se ruim, ou pular para pegar se boa
};

/**
 * @brief Classe base abstrata para itens coletáveis de comida.
 *
 * Utiliza o paradigma de Orientação a Objetos (Herança e Polimorfismo)
 * para definir comportamento comum de colisão, física e renderização 3D.
 */
class Food
{
public:
    virtual ~Food() = default;

    virtual FoodType GetType() const = 0;
    virtual void ApplyTo(Player& player) const = 0;
    virtual Color GetColor() const = 0;
    virtual const char* GetDisplayName() const = 0;
    virtual float GetWeightValue() const = 0;
    virtual int GetScoreBonus() const = 0;
    virtual void DrawModel(Vector3 pos, float rotationAngle) const = 0;

    void SetPosition(Vector3 newPosition);
    Vector3 GetPosition() const;
    bool IsActive() const;
    void SetActive(bool isActive);
    BoundingBox GetBoundingBox() const;

    void Update(float deltaTime);
    void Draw() const;

    FoodAltitude GetAltitude() const;
    void SetAltitude(FoodAltitude alt);

protected:
    Vector3 position{};
    bool active = false;
    float rotation = 0.0F;
    FoodAltitude altitude = FoodAltitude::Ground;
};

// ==========================================
// COMIDAS SAUDÁVEIS (Reduzem peso / Pontos)
// ==========================================
class GoodFood : public Food
{
public:
    FoodType GetType() const override { return FoodType::Good; }
    void ApplyTo(Player& player) const override;
    Color GetColor() const override;
};

class Apple final : public GoodFood
{
public:
    const char* GetDisplayName() const override;
    float GetWeightValue() const override;
    int GetScoreBonus() const override;
    void DrawModel(Vector3 pos, float rotationAngle) const override;
};

class Carrot final : public GoodFood
{
public:
    const char* GetDisplayName() const override;
    float GetWeightValue() const override;
    int GetScoreBonus() const override;
    void DrawModel(Vector3 pos, float rotationAngle) const override;
};

class GrilledSteak final : public GoodFood
{
public:
    const char* GetDisplayName() const override;
    float GetWeightValue() const override;
    int GetScoreBonus() const override;
    void DrawModel(Vector3 pos, float rotationAngle) const override;
};

// ==========================================
// COMIDAS RUINS (Aumentam peso / Morte)
// ==========================================
class BadFood : public Food
{
public:
    FoodType GetType() const override { return FoodType::Bad; }
    void ApplyTo(Player& player) const override;
    Color GetColor() const override;
};

class Burger final : public BadFood
{
public:
    const char* GetDisplayName() const override;
    float GetWeightValue() const override;
    int GetScoreBonus() const override;
    void DrawModel(Vector3 pos, float rotationAngle) const override;
};

class Fries final : public BadFood
{
public:
    const char* GetDisplayName() const override;
    float GetWeightValue() const override;
    int GetScoreBonus() const override;
    void DrawModel(Vector3 pos, float rotationAngle) const override;
};

class SodaCan final : public BadFood
{
public:
    const char* GetDisplayName() const override;
    float GetWeightValue() const override;
    int GetScoreBonus() const override;
    void DrawModel(Vector3 pos, float rotationAngle) const override;
};

class Donut final : public BadFood
{
public:
    const char* GetDisplayName() const override;
    float GetWeightValue() const override;
    int GetScoreBonus() const override;
    void DrawModel(Vector3 pos, float rotationAngle) const override;
};
