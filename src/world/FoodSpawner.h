#pragma once

#include <memory>
#include <vector>

#include "entities/Food.h"

/**
 * @brief Gerenciador e gerador procedural de comidas nas raias da pista.
 *
 * Utiliza o padrão Object Pooling com std::unique_ptr para reaproveitar
 * instâncias de alimentos saudáveis e não saudáveis sem alocações dinâmicas
 * custosas durante o gameplay.
 */
class FoodSpawner
{
public:
    FoodSpawner();

    void Update(float deltaTime, float speed, float badFoodChance, float spawnInterval);
    void Draw() const;
    std::vector<std::unique_ptr<Food>>& GetFoods();
    void Reset();

private:
    std::vector<std::unique_ptr<Food>> foods;
    float distanceUntilNextSpawn = 15.0F;
    int previousSafeLane = -2;

    void SpawnFood(float badFoodChance);
    Food* FindInactiveFood(FoodType type);
    void SpawnFoodInLane(FoodType type, int lane);
};
