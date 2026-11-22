#include "DifficultyManager.h"

float DifficultyManager::GetSpeed(float distance) const
{
    if (distance < 200.0F)
    {
        return 8.0F;
    }
    if (distance < 600.0F)
    {
        return 11.0F;
    }
    if (distance < 1200.0F)
    {
        return 14.0F;
    }
    return 17.0F;
}

float DifficultyManager::GetBadFoodChance(float distance) const
{
    if (distance < 200.0F)
    {
        return 0.20F;
    }
    if (distance < 600.0F)
    {
        return 0.40F;
    }
    if (distance < 1200.0F)
    {
        return 0.60F;
    }
    return 0.80F;
}
