#pragma once

/**
 * @brief Gerenciador de Dificuldade Progressiva.
 *
 * Ajusta dinamicamente a velocidade do mundo e a proporção de alimentos
 * calóricos gerados em função da distância percorrida pelo jogador.
 */
class DifficultyManager
{
public:
    float GetSpeed(float distance) const;
    float GetBadFoodChance(float distance) const;
    float GetSpawnInterval(float distance) const;
};
