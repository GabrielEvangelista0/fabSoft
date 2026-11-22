#include "FoodSpawner.h"

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

void FoodSpawner::Update(float deltaTime, float speed, float badFoodChance)
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
        // Distância entre spawns decresce levemente com a velocidade para mais ritmo
        distanceUntilNextSpawn += static_cast<float>(GetRandomValue(10, 16));
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
}

void FoodSpawner::SpawnFood(float badFoodChance)
{
    const bool spawnBadFood = static_cast<float>(GetRandomValue(0, 100)) < (badFoodChance * 100.0F);
    const FoodType desiredType = spawnBadFood ? FoodType::Bad : FoodType::Good;

    // Procura por itens inativos do tipo desejado
    std::vector<size_t> availableIndices;
    for (size_t i = 0; i < foods.size(); ++i)
    {
        if (!foods[i]->IsActive() && foods[i]->GetType() == desiredType)
        {
            availableIndices.push_back(i);
        }
    }

    if (availableIndices.empty())
    {
        return;
    }

    // Seleciona aleatoriamente um item do pool disponível
    const size_t chosenIdx = availableIndices[static_cast<size_t>(GetRandomValue(0, static_cast<int>(availableIndices.size() - 1)))];
    Food* food = foods[chosenIdx].get();

    const int lane = GetRandomValue(-1, 1);
    float altitudeY = 0.45F;
    FoodAltitude altitudeType = FoodAltitude::Ground;

    if (desiredType == FoodType::Bad)
    {
        // 50% chance de obstáculo no chão (pular/desviar), 50% de obstáculo alto (agachar/desviar)
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
        // 75% no chão, 25% suspenso no ar (recompensa por pulo calculado)
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
        static_cast<float>(lane) * Settings::LANE_WIDTH,
        altitudeY,
        48.0F
    });
    food->SetActive(true);
}
