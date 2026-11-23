#include "FoodSystem.h"

#include <algorithm>

#include "core/Settings.h"

FoodSystem::FoodSystem()
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

void FoodSystem::Update(float deltaTime, float speed, float badFoodChance, float spawnInterval, float currentWorldZ)
{
    worldZ = currentWorldZ;
    const float distanceTravelled = speed * deltaTime;
    (void)distanceTravelled;

    for (const std::unique_ptr<Food>& food : foods)
    {
        if (food->IsActive())
        {
            food->Update(deltaTime);
            Vector3 position = food->GetPosition();
            food->SetPosition(position);
            if (position.z < worldZ - 4.0F)
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

void FoodSystem::Draw() const
{
    for (const std::unique_ptr<Food>& food : foods)
    {
        food->Draw();
    }
}

const std::vector<std::unique_ptr<Food>>& FoodSystem::GetActiveFoods() const
{
    return foods;
}

void FoodSystem::Reset()
{
    for (const std::unique_ptr<Food>& food : foods)
    {
        food->SetActive(false);
    }
    distanceUntilNextSpawn = 15.0F;
    previousSafeLane = -2;
}

void FoodSystem::ResolveCollisions(Player& player, ScoreSystem& score)
{
    collisionEvents.clear();
    for (const std::unique_ptr<Food>& food : foods)
    {
        if (!food->IsActive() ||
            !CheckCollisionBoxes(player.GetBoundingBox(), food->GetBoundingBox()))
        {
            continue;
        }

        const bool isGoodFood = food->GetType() == FoodType::Good;
        const Vector3 position = food->GetPosition();
        food->ApplyTo(player);
        food->SetActive(false);

        if (isGoodFood)
        {
            const float comboMultiplier = score.RegisterGoodFood();
            const int points = static_cast<int>(
                static_cast<float>(food->GetScoreBonus()) * comboMultiplier);
            score.AddFoodBonus(points);
            collisionEvents.push_back({
                true, position, food->GetWeightValue(), points,
                comboMultiplier, food->GetDisplayName()});
        }
        else
        {
            score.RegisterBadFood();
            collisionEvents.push_back({
                false, position, food->GetWeightValue(), 0,
                1.0F, food->GetDisplayName()});
        }
    }
}

const std::vector<FoodSystem::CollisionEvent>&
FoodSystem::GetCollisionEvents() const
{
    return collisionEvents;
}

void FoodSystem::SpawnFood(float badFoodChance)
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

Food* FoodSystem::FindInactiveFood(FoodType type)
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

void FoodSystem::SpawnFoodInLane(FoodType type, int lane)
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
        worldZ + 48.0F
    });
    food->SetActive(true);
}
