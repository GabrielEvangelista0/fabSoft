#include "ScoreSystem.h"

#include <algorithm>

void ScoreSystem::Reset()
{
    distance = 0.0F;
    foodBonus = 0;
    goodFoodCount = 0;
    badFoodCount = 0;
}

void ScoreSystem::Update(float deltaTime, float speed)
{
    distance += std::max(0.0F, deltaTime) * speed;
}

void ScoreSystem::AddFoodBonus(int bonus)
{
    foodBonus += bonus;
}

void ScoreSystem::RegisterGoodFood()
{
    goodFoodCount++;
}

void ScoreSystem::RegisterBadFood()
{
    badFoodCount++;
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
