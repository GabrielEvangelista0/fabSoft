#include "SaveSystem.h"

#include <fstream>
#include <iostream>

namespace
{
constexpr const char* HIGH_SCORE_FILE = "highscore.txt";
}

int SaveSystem::LoadHighScore() const
{
    std::ifstream file(HIGH_SCORE_FILE);
    if (!file)
    {
        return 0;
    }

    int score = 0;
    if (!(file >> score) || score < 0)
    {
        std::cerr << "Aviso: nao foi possivel ler highscore.txt; usando 0.\n";
        return 0;
    }

    return score;
}

bool SaveSystem::SaveHighScore(int score) const
{
    std::ofstream file(HIGH_SCORE_FILE, std::ios::trunc);
    if (!file)
    {
        std::cerr << "Erro: nao foi possivel gravar highscore.txt.\n";
        return false;
    }

    file << score << '\n';
    if (!file)
    {
        std::cerr << "Erro: falha ao gravar highscore.txt.\n";
        return false;
    }

    return true;
}
