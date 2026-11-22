#pragma once

#include "raylib.h"

/**
 * @brief Gerenciador de Áudio com síntese procedural.
 *
 * Gera ondas sonoras em tempo de execução via PCM, dispensando
 * arquivos externos de áudio (.wav/.mp3), garantindo portabilidade
 * total e robustez na execução.
 */
class AudioManager
{
public:
    AudioManager();
    ~AudioManager();

    // Impede cópia (RAII dos recursos de áudio)
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    void PlayJump();
    void PlayCollectGood();
    void PlayCollectBad();
    void PlayGameOver();

    void ToggleMute();
    bool IsMuted() const;

private:
    bool audioReady = false;
    bool muted = false;

    Sound sndJump{};
    Sound sndGood{};
    Sound sndBad{};
    Sound sndGameOver{};

    Sound GenerateJumpSound();
    Sound GenerateGoodSound();
    Sound GenerateBadSound();
    Sound GenerateGameOverSound();
};
