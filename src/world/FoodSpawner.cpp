#include "FoodSpawner.h"

#include <algorithm>

#include "core/Settings.h"

FoodSpawner::FoodSpawner()
{
    foods.reserve(Settings::MAX_FOODS);

    // Cria pool diversificado de comidas saudáveis e não saudáveis
    for (int i = 0; i < 8; ++i)
    {
        foods.push_back(std::make_unique<Apple>());
        foods.push_back(std::make_unique<Carrot>());
        foods.push_back(std::make_unique<GrilledSteak>());

        foods.push_back(std::make_unique<Burger>());
        foods.push_back(std::make_unique<Fries>());
        foods.push_back(std::make_unique<SodaCan>());
        foods.push_back(std::make_unique<Donut>());
    }
}

void FoodSpawner::Update(float deltaTime, float speed, float badFoodChance, float spawnInterval)
{
    const float distanceTravelled = speed * deltaTime;

    for (const std::unique_ptr<Food>& food : foods)
    {
        if (food->IsActive())
        {
            food->Update(deltaTime);
            Vector3 position = food->GetPosition();
            position.z -= distanceTravelled;
            food->SetPosition(position);
            if (position.z < -4.0F)
            {
                food->SetActive(false);
            }
        }
    }

    distanceUntilNextSpawn -= distanceTravelled;
    while (distanceUntilNextSpawn <= 0.0F)
    {
        SpawnFood(badFoodChance);
        const float spacingVariation = static_cast<float>(GetRandomValue(-10, 10)) * 0.01F;
        distanceUntilNextSpawn += std::max(10.0F, spawnInterval * (1.0F + spacingVariation));
    }
}

void FoodSpawner::Draw() const
{
    for (const std::unique_ptr<Food>& food : foods)
    {
        food->Draw();
    }
}

std::vector<std::unique_ptr<Food>>& FoodSpawner::GetFoods()
{
    return foods;
}

void FoodSpawner::Reset()
{
    for (const std::unique_ptr<Food>& food : foods)
    {
        food->SetActive(false);
    }
    distanceUntilNextSpawn = 15.0F;
    previousSafeLane = -2;
}

void FoodSpawner::SpawnFood(float badFoodChance)
{
    int lanes[] = {-1, 0, 1};
    const int firstLaneIndex = GetRandomValue(0, 2);
    std::swap(lanes[0], lanes[firstLaneIndex]);
    badFoodChance = std::clamp(badFoodChance, 0.0F, 1.0F);

    if (GetRandomValue(0, 999) >= static_cast<int>(badFoodChance * 1000.0F))
    {
        SpawnFoodInLane(FoodType::Good, lanes[0]);
        previousSafeLane = -2;
        return;
    }

    if (previousSafeLane >= -1 && previousSafeLane <= 1)
    {
        for (int i = 1; i < 3; ++i)
        {
            if (lanes[i] == previousSafeLane)
            {
                std::swap(lanes[0], lanes[i]);
                break;
            }
        }
    }

    SpawnFoodInLane(FoodType::Bad, lanes[0]);

    const float secondObstacleChance = std::clamp((badFoodChance - 0.30F) * 2.0F, 0.0F, 0.70F);
    const bool spawnSecondObstacle = GetRandomValue(0, 999) <
        static_cast<int>(secondObstacleChance * 1000.0F);
    int safeLane = lanes[GetRandomValue(1, 2)];
    if (spawnSecondObstacle)
    {
        SpawnFoodInLane(FoodType::Bad, lanes[1]);
        safeLane = lanes[2];
    }

    previousSafeLane = safeLane;
    if (GetRandomValue(0, 99) < 35)
    {
        SpawnFoodInLane(FoodType::Good, safeLane);
    }
}

Food* FoodSpawner::FindInactiveFood(FoodType type)
{
    for (const std::unique_ptr<Food>& food : foods)
    {
        if (!food->IsActive() && food->GetType() == type)
        {
            return food.get();
        }
    }
    return nullptr;
}

void FoodSpawner::SpawnFoodInLane(FoodType type, int lane)
{
    Food* food = FindInactiveFood(type);
    if (food == nullptr)
    {
        return;
    }

    float altitudeY = 0.45F;
    FoodAltitude altitudeType = FoodAltitude::Ground;

    if (type == FoodType::Bad)
    {
        if (GetRandomValue(0, 1) == 1)
        {
            altitudeY = 1.55F;
            altitudeType = FoodAltitude::Suspended;
        }
        else
        {
            altitudeY = 0.45F;
            altitudeType = FoodAltitude::Ground;
        }
    }
    else
    {
        if (GetRandomValue(0, 3) == 3)
        {
            altitudeY = 1.70F;
            altitudeType = FoodAltitude::Suspended;
        }
        else
        {
            altitudeY = 0.45F;
            altitudeType = FoodAltitude::Ground;
        }
    }

    food->SetAltitude(altitudeType);
    food->SetPosition({
        -static_cast<float>(lane) * Settings::LANE_WIDTH,
        altitudeY,
        48.0F
    });
    food->SetActive(true);
}
