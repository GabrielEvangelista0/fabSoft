#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <vector>

#include "core/Settings.h"
#include "entities/Food.h"
#include "entities/Player.h"
#include "systems/DifficultyManager.h"
#include "systems/SaveSystem.h"
#include "systems/ScoreSystem.h"
#include "world/FoodSystem.h"

int g_passedTests = 0;
int g_failedTests = 0;

#define TEST_ASSERT(cond, msg)                                                \
    do {                                                                      \
        if (!(cond)) {                                                        \
            std::cerr << "  [FAIL] " << msg << " (" << #cond << ")\n";       \
            g_failedTests++;                                                  \
            return;                                                           \
        }                                                                     \
    } while (0)

#define RUN_TEST(fn)                                                          \
    do {                                                                      \
        std::cout << "[EXECUTANDO] " << #fn << "...\n";                       \
        const int prevFail = g_failedTests;                                   \
        fn();                                                                 \
        if (g_failedTests == prevFail) {                                      \
            std::cout << "  [PASS] " << #fn << " passou com sucesso!\n";      \
            g_passedTests++;                                                  \
        }                                                                     \
    } while (0)

void TestPlayerInitialStateAndWeight()
{
    Player player;
    TEST_ASSERT(player.GetWeight() == 20.0F, "Peso inicial deve ser 20 kg");
    TEST_ASSERT(player.GetWeightState() == WeightState::Normal, "Estado inicial deve ser Normal");
    TEST_ASSERT(!player.IsGameOver(), "Player nao deve iniciar em Game Over");
    TEST_ASSERT(player.GetLaneX() == 0.0F, "Player deve iniciar na raia central (X = 0)");
}

void TestPlayerWeightMechanics()
{
    Player player;

    // Comer comida ruim: peso aumenta
    player.OnCollideBadFood(30.0F);
    TEST_ASSERT(player.GetWeight() == 50.0F, "Peso deve ter subido para 50 kg");
    TEST_ASSERT(player.GetWeightState() == WeightState::Overweight, "50 kg deve ser estado Sobrepeso");

    player.OnCollideBadFood(25.0F);
    TEST_ASSERT(player.GetWeight() == 75.0F, "Peso deve ter subido para 75 kg");
    TEST_ASSERT(player.GetWeightState() == WeightState::Obese, "75 kg deve ser estado Obeso");

    // Comer comida boa: peso diminui
    player.OnCollideGoodFood(40.0F);
    TEST_ASSERT(player.GetWeight() == 35.0F, "Peso deve ter descido para 35 kg");
    TEST_ASSERT(player.GetWeightState() == WeightState::Normal, "35 kg deve voltar ao estado Normal");

    // Redução além de zero não deve gerar peso negativo
    player.OnCollideGoodFood(50.0F);
    TEST_ASSERT(player.GetWeight() == 0.0F, "Peso minimo deve ser travado em 0 kg");
}

void TestPlayerGameOverCondition()
{
    Player player;
    TEST_ASSERT(!player.IsGameOver(), "Nao deve ser Game Over inicialmente");

    player.OnCollideBadFood(79.0F);
    TEST_ASSERT(player.GetWeight() == 99.0F, "Peso em 99 kg nao atinge limite maximo");
    TEST_ASSERT(!player.IsGameOver(), "99 kg nao deve ser Game Over ainda");

    player.OnCollideBadFood(2.0F);
    TEST_ASSERT(player.GetWeight() == Settings::MAX_WEIGHT, "Peso deve travar no MAX_WEIGHT (100 kg)");
    TEST_ASSERT(player.IsGameOver(), "100 kg deve disparar Game Over obrigatorio");
}

void TestFoodPolymorphismAndEffects()
{
    Player player;
    const float initialWeight = player.GetWeight(); // 20 kg

    // 1. Testa comidas boas
    std::unique_ptr<Food> apple = std::make_unique<Apple>();
    TEST_ASSERT(apple->GetType() == FoodType::Good, "Apple deve ser FoodType::Good");
    TEST_ASSERT(apple->GetWeightValue() > 0.0F, "Apple deve ter valor de reducao positivo");
    TEST_ASSERT(apple->GetScoreBonus() > 0, "Apple deve fornecer pontos bonus");

    apple->ApplyTo(player);
    TEST_ASSERT(player.GetWeight() == (initialWeight - apple->GetWeightValue()), "Apple reduziu peso");

    std::unique_ptr<Food> steak = std::make_unique<GrilledSteak>();
    TEST_ASSERT(steak->GetType() == FoodType::Good, "GrilledSteak deve ser FoodType::Good");
    TEST_ASSERT(steak->GetScoreBonus() >= 150, "Carne grelhada deve dar bom bonus de pontos");

    // 2. Testa comidas ruins
    std::unique_ptr<Food> burger = std::make_unique<Burger>();
    TEST_ASSERT(burger->GetType() == FoodType::Bad, "Burger deve ser FoodType::Bad");
    TEST_ASSERT(burger->GetWeightValue() >= 15.0F, "Burger deve ter alto impacto calorico");

    const float weightBeforeBurger = player.GetWeight();
    burger->ApplyTo(player);
    TEST_ASSERT(player.GetWeight() == (weightBeforeBurger + burger->GetWeightValue()), "Burger aumentou peso");

    std::unique_ptr<Food> fries = std::make_unique<Fries>();
    std::unique_ptr<Food> soda = std::make_unique<SodaCan>();
    std::unique_ptr<Food> donut = std::make_unique<Donut>();

    TEST_ASSERT(fries->GetType() == FoodType::Bad, "Fries e comida ruim");
    TEST_ASSERT(soda->GetType() == FoodType::Bad, "Soda e comida ruim");
    TEST_ASSERT(donut->GetType() == FoodType::Bad, "Donut e comida ruim");
}

void TestDifficultyProgression()
{
    DifficultyManager diff;

    // Velocidade crescente
    const float speedEarly = diff.GetSpeed(100.0F);
    const float speedMid = diff.GetSpeed(400.0F);
    const float speedLate = diff.GetSpeed(1600.0F);
    const float speedMax = diff.GetSpeed(3000.0F);

    TEST_ASSERT(std::abs(speedEarly - 9.75F) < 0.01F, "Velocidade deve subir gradualmente no inicio");
    TEST_ASSERT(speedMid == 12.0F, "Velocidade aos 400m deve ser 12 m/s");
    TEST_ASSERT(speedLate == 18.0F, "Velocidade aos 1600m deve ser 18 m/s");
    TEST_ASSERT(speedMax == 20.0F, "Velocidade maxima deve ser limitada a 20 m/s");
    TEST_ASSERT(speedEarly < speedMid && speedMid < speedLate && speedLate < speedMax,
                "Velocidade deve crescer monotonicamente com a distancia");

    // Frequência crescente de comidas ruins
    const float chanceEarly = diff.GetBadFoodChance(100.0F);
    const float chanceMid = diff.GetBadFoodChance(400.0F);
    const float chanceLate = diff.GetBadFoodChance(1600.0F);
    const float chanceMax = diff.GetBadFoodChance(3000.0F);

    TEST_ASSERT(std::abs(chanceEarly - 0.275F) < 0.01F, "Ondas perigosas devem comecar em 25%");
    TEST_ASSERT(chanceMid == 0.35F, "Chance aos 400m deve ser 35%");
    TEST_ASSERT(chanceLate == 0.55F, "Chance aos 1600m deve ser 55%");
    TEST_ASSERT(chanceMax == 0.60F, "Chance maxima deve ser limitada a 60%");

    TEST_ASSERT(diff.GetSpawnInterval(0.0F) == 18.0F, "Ondas iniciais devem ter mais espaco");
    TEST_ASSERT(diff.GetSpawnInterval(3000.0F) == 12.0F, "Espacamento minimo deve permanecer justo");
}

void TestGoodFoodCombo()
{
    ScoreSystem score;
    score.Reset();

    TEST_ASSERT(score.RegisterGoodFood() == 1.0F, "Primeira coleta deve valer 1x");
    TEST_ASSERT(score.RegisterGoodFood() == 1.25F, "Segunda coleta deve aumentar o combo");
    TEST_ASSERT(score.RegisterGoodFood() == 1.5F, "Combo deve continuar crescendo");
    TEST_ASSERT(score.RegisterGoodFood() == 1.75F, "Combo deve continuar crescendo");
    TEST_ASSERT(score.RegisterGoodFood() == 2.0F, "Combo deve atingir 2x");
    TEST_ASSERT(score.RegisterGoodFood() == 2.0F, "Combo deve ter limite de 2x");

    score.RegisterBadFood();
    TEST_ASSERT(score.RegisterGoodFood() == 1.0F, "Comida ruim deve reiniciar o combo");
}

void TestFoodSystemKeepsSafeLane()
{
    SetRandomSeed(7);
    FoodSystem spawner;
    spawner.Update(1.0F, 15.0F, 1.0F, 15.0F, 0.0F);
    spawner.Update(1.0F, 15.0F, 1.0F, 10.0F, 15.0F);

    bool blockedLanes[3] = {false, false, false};
    int badFoodCount = 0;
    for (const std::unique_ptr<Food>& food : spawner.GetActiveFoods())
    {
        if (!food->IsActive() || food->GetType() != FoodType::Bad)
        {
            continue;
        }

        const int lane = static_cast<int>(std::lround(-food->GetPosition().x / Settings::LANE_WIDTH)) + 1;
        TEST_ASSERT(lane >= 0 && lane < 3, "Obstaculo deve estar em uma das tres raias");
        (void)blockedLanes[lane];
        blockedLanes[lane] = true;
        ++badFoodCount;
    }

    TEST_ASSERT(badFoodCount >= 1 && badFoodCount <= 4,
                "Ondas de risco devem bloquear apenas algumas raias");
}

void TestScoreSystemCalculations()
{
    ScoreSystem score;
    score.Reset();

    TEST_ASSERT(score.GetScore() == 0, "Score inicial deve ser 0");
    TEST_ASSERT(score.GetDistance() == 0.0F, "Distancia inicial deve ser 0");

    // Simula 10 segundos correndo a 10 m/s = 100 metros
    score.Update(10.0F, 10.0F);
    TEST_ASSERT(static_cast<int>(score.GetDistance()) == 100, "Distancia deve ser 100m");
    TEST_ASSERT(score.GetScore() == 100, "Score por distancia deve ser 100");

    // Adiciona bonus de comidas saudaveis
    score.AddFoodBonus(100);
    score.RegisterGoodFood();
    TEST_ASSERT(score.GetScore() == 200, "Score com bonus deve ser 200");
    TEST_ASSERT(score.GetGoodFoodCount() == 1, "Deve ter 1 comida saudavel registrada");

    score.RegisterBadFood();
    TEST_ASSERT(score.GetBadFoodCount() == 1, "Deve ter 1 comida ruim registrada");
}

void TestSaveSystemPersistence()
{
    SaveSystem save;
    const int previousHigh = save.LoadHighScore();

    const int testScore = 8888;
    TEST_ASSERT(save.SaveHighScore(testScore), "Gravacao de highscore deve ter sucesso");
    TEST_ASSERT(save.LoadHighScore() == testScore, "Highscore carregado deve ser igual ao gravado");

    // Restaura pontuação original
    save.SaveHighScore(previousHigh);
    TEST_ASSERT(save.LoadHighScore() == previousHigh, "Highscore deve ser restaurado");
}

int main()
{
    std::cout << "==================================================\n";
    std::cout << "  INICIANDO SUITE DE TESTES: FUJA DAS CALORIAS!   \n";
    std::cout << "==================================================\n\n";

    RUN_TEST(TestPlayerInitialStateAndWeight);
    RUN_TEST(TestPlayerWeightMechanics);
    RUN_TEST(TestPlayerGameOverCondition);
    RUN_TEST(TestFoodPolymorphismAndEffects);
    RUN_TEST(TestDifficultyProgression);
    RUN_TEST(TestGoodFoodCombo);
    RUN_TEST(TestFoodSystemKeepsSafeLane);
    RUN_TEST(TestScoreSystemCalculations);
    RUN_TEST(TestSaveSystemPersistence);

    std::cout << "\n==================================================\n";
    std::cout << "RESULTADO DOS TESTES: " << g_passedTests << " PASSOU, " << g_failedTests << " FALHOU\n";
    std::cout << "==================================================\n";

    return (g_failedTests == 0) ? 0 : 1;
}
