#include "DifficultyManager.h"

#include <algorithm>

float DifficultyManager::GetSpeed(float distance) const
{
    distance = std::max(0.0F, distance);
    if (distance < 400.0F)
    {
        return 9.0F + distance * (3.0F / 400.0F);
    }
    if (distance < 900.0F)
    {
        return 12.0F + (distance - 400.0F) * (3.0F / 500.0F);
    }
    if (distance < 1600.0F)
    {
        return 15.0F + (distance - 900.0F) * (3.0F / 700.0F);
    }
    if (distance < 2400.0F)
    {
        return 18.0F + (distance - 1600.0F) * (2.0F / 800.0F);
    }
    return 20.0F;
}

float DifficultyManager::GetBadFoodChance(float distance) const
{
    distance = std::max(0.0F, distance);
    if (distance < 400.0F)
    {
        return 0.25F + distance * (0.10F / 400.0F);
    }
    if (distance < 900.0F)
    {
        return 0.35F + (distance - 400.0F) * (0.10F / 500.0F);
    }
    if (distance < 1600.0F)
    {
        return 0.45F + (distance - 900.0F) * (0.10F / 700.0F);
    }
    if (distance < 2400.0F)
    {
        return 0.55F + (distance - 1600.0F) * (0.05F / 800.0F);
    }
    return 0.60F;
}

float DifficultyManager::GetSpawnInterval(float distance) const
{
    distance = std::max(0.0F, distance);
    if (distance < 400.0F)
    {
        return 18.0F - distance * (2.0F / 400.0F);
    }
    if (distance < 900.0F)
    {
        return 16.0F - (distance - 400.0F) * (2.0F / 500.0F);
    }
    if (distance < 1600.0F)
    {
        return 14.0F - (distance - 900.0F) * (1.5F / 700.0F);
    }
    if (distance < 2400.0F)
    {
        return 12.5F - (distance - 1600.0F) * (0.5F / 800.0F);
    }
    return 12.0F;
}
