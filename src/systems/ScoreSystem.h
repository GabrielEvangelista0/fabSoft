#pragma once

/**
 * @brief Sistema de pontuação e estatísticas da partida.
 *
 * Registra a distância percorrida, bônus obtidos por coleta de alimentos
 * saudáveis e contadores de itens consumidos para exibição no Game Over.
 */
class ScoreSystem
{
public:
    void Reset();
    void Update(float deltaTime, float speed);
    void AddFoodBonus(int bonus);
    float RegisterGoodFood();
    void RegisterBadFood();

    float GetDistance() const;
    int GetScore() const;
    int GetGoodFoodCount() const;
    int GetBadFoodCount() const;
    float GetGoodFoodComboMultiplier() const;

private:
    float distance = 0.0F;
    int foodBonus = 0;
    int goodFoodCount = 0;
    int badFoodCount = 0;
    int goodFoodStreak = 0;
};
