#pragma once

#include <memory>
#include <vector>

#include "entities/Food.h"
#include "entities/Player.h"
#include "systems/ScoreSystem.h"

/**
 * @brief Gerenciador e gerador procedural de comidas nas raias da pista.
 *
 * Utiliza o padrão Object Pooling com std::unique_ptr para reaproveitar
 * instâncias de alimentos saudáveis e não saudáveis sem alocações dinâmicas
 * custosas durante o gameplay.
 */
class FoodSystem
{
public:
    FoodSystem();

    void Update(float deltaTime, float speed, float badFoodChance, float spawnInterval, float worldZ);
    void Draw() const;
    const std::vector<std::unique_ptr<Food>>& GetActiveFoods() const;
    void Reset();
    struct CollisionEvent
    {
        bool isGood = false;
        Vector3 position{};
        float weightValue = 0.0F;
        int points = 0;
        float comboMultiplier = 1.0F;
        const char* displayName = "";
    };

    void ResolveCollisions(Player& player, ScoreSystem& score);
    const std::vector<CollisionEvent>& GetCollisionEvents() const;

private:
    std::vector<std::unique_ptr<Food>> foods;
    float distanceUntilNextSpawn = 15.0F;
    float worldZ = 0.0F;
    int previousSafeLane = -2;
    std::vector<CollisionEvent> collisionEvents;

    void SpawnFood(float badFoodChance);
    Food* FindInactiveFood(FoodType type);
    void SpawnFoodInLane(FoodType type, int lane);
};

