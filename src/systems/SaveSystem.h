#pragma once

/**
 * @brief Sistema de Persistência Local (High Score).
 *
 * Salva e carrega o recorde de pontuação em arquivo de texto local (highscore.txt),
 * garantindo a preservação do melhor resultado entre sessões de jogo.
 */
class SaveSystem
{
public:
    int LoadHighScore() const;
    bool SaveHighScore(int score) const;
};
