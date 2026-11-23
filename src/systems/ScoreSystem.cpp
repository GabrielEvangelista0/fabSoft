#include "ScoreSystem.h"

#include <algorithm>

void ScoreSystem::Reset()
{
    distance = 0.0F;
    foodBonus = 0;
    goodFoodCount = 0;
    badFoodCount = 0;
    goodFoodStreak = 0;
}

void ScoreSystem::Update(float deltaTime, float speed)
{
    distance += std::max(0.0F, deltaTime) * speed;
}

void ScoreSystem::AddFoodBonus(int bonus)
{
    foodBonus += bonus;
}

float ScoreSystem::RegisterGoodFood()
{
    goodFoodCount++;
    ++goodFoodStreak;
    return GetGoodFoodComboMultiplier();
}

void ScoreSystem::RegisterBadFood()
{
    badFoodCount++;
    goodFoodStreak = 0;
}

float ScoreSystem::GetDistance() const
{
    return distance;
}

int ScoreSystem::GetScore() const
{
    return static_cast<int>(distance) + foodBonus;
}

int ScoreSystem::GetGoodFoodCount() const
{
    return goodFoodCount;
}

int ScoreSystem::GetBadFoodCount() const
{
    return badFoodCount;
}

float ScoreSystem::GetGoodFoodComboMultiplier() const
{
    const int bonusSteps = std::min(std::max(0, goodFoodStreak - 1), 4);
    return 1.0F + static_cast<float>(bonusSteps) * 0.25F;
}
