#include "AudioManager.h"

#include <cmath>
#include <cstdlib>
#include <vector>

namespace
{
constexpr unsigned int SAMPLE_RATE = 44100;
constexpr float PI_CONST = 3.14159265358979323846F;

Sound CreateSoundFromSamples(const std::vector<short>& samples)
{
    Wave wave{};
    wave.frameCount = static_cast<unsigned int>(samples.size());
    wave.sampleRate = SAMPLE_RATE;
    wave.sampleSize = 16;
    wave.channels = 1;

    // Aloca cópia para Raylib gerenciar/desalocar com UnloadWave
    wave.data = MemAlloc(static_cast<unsigned int>(samples.size() * sizeof(short)));
    if (wave.data != nullptr)
    {
        std::memcpy(wave.data, samples.data(), samples.size() * sizeof(short));
    }

    Sound sound = LoadSoundFromWave(wave);
    UnloadWave(wave);
    return sound;
}
}

AudioManager::AudioManager()
{
    InitAudioDevice();
    audioReady = IsAudioDeviceReady();

    if (audioReady)
    {
        sndJump = GenerateJumpSound();
        sndGood = GenerateGoodSound();
        sndBad = GenerateBadSound();
        sndGameOver = GenerateGameOverSound();

        SetSoundVolume(sndJump, 0.7F);
        SetSoundVolume(sndGood, 0.8F);
        SetSoundVolume(sndBad, 0.8F);
        SetSoundVolume(sndGameOver, 0.9F);
    }
}

AudioManager::~AudioManager()
{
    if (audioReady)
    {
        UnloadSound(sndJump);
        UnloadSound(sndGood);
        UnloadSound(sndBad);
        UnloadSound(sndGameOver);
        CloseAudioDevice();
    }
}

void AudioManager::PlayJump()
{
    if (audioReady && !muted)
    {
        PlaySound(sndJump);
    }
}

void AudioManager::PlayCollectGood()
{
    if (audioReady && !muted)
    {
        PlaySound(sndGood);
    }
}

void AudioManager::PlayCollectBad()
{
    if (audioReady && !muted)
    {
        PlaySound(sndBad);
    }
}

void AudioManager::PlayGameOver()
{
    if (audioReady && !muted)
    {
        PlaySound(sndGameOver);
    }
}

void AudioManager::ToggleMute()
{
    muted = !muted;
}

bool AudioManager::IsMuted() const
{
    return muted;
}

Sound AudioManager::GenerateJumpSound()
{
    const float duration = 0.15F;
    const size_t totalSamples = static_cast<size_t>(SAMPLE_RATE * duration);
    std::vector<short> samples(totalSamples);

    float phase = 0.0F;
    for (size_t i = 0; i < totalSamples; ++i)
    {
        const float t = static_cast<float>(i) / static_cast<float>(totalSamples);
        const float freq = 260.0F + (620.0F - 260.0F) * t;
        phase += 2.0F * PI_CONST * freq / static_cast<float>(SAMPLE_RATE);

        const float envelope = (1.0F - t);
        const float sampleVal = std::sin(phase) * envelope * 24000.0F;
        samples[i] = static_cast<short>(sampleVal);
    }

    return CreateSoundFromSamples(samples);
}

Sound AudioManager::GenerateGoodSound()
{
    const float duration = 0.28F;
    const size_t totalSamples = static_cast<size_t>(SAMPLE_RATE * duration);
    std::vector<short> samples(totalSamples);

    const size_t noteSplit = totalSamples / 2;
    float phase1 = 0.0F;
    float phase2 = 0.0F;

    for (size_t i = 0; i < totalSamples; ++i)
    {
        float val = 0.0F;
        if (i < noteSplit)
        {
            const float t = static_cast<float>(i) / static_cast<float>(noteSplit);
            phase1 += 2.0F * PI_CONST * 587.33F / static_cast<float>(SAMPLE_RATE); // D5
            const float env = (1.0F - t * 0.5F);
            val = (std::sin(phase1) + 0.3F * std::sin(phase1 * 2.0F)) * env;
        }
        else
        {
            const float t = static_cast<float>(i - noteSplit) / static_cast<float>(totalSamples - noteSplit);
            phase2 += 2.0F * PI_CONST * 880.0F / static_cast<float>(SAMPLE_RATE); // A5
            const float env = (1.0F - t);
            val = (std::sin(phase2) + 0.25F * std::sin(phase2 * 2.0F)) * env;
        }

        samples[i] = static_cast<short>(val * 22000.0F);
    }

    return CreateSoundFromSamples(samples);
}

Sound AudioManager::GenerateBadSound()
{
    const float duration = 0.25F;
    const size_t totalSamples = static_cast<size_t>(SAMPLE_RATE * duration);
    std::vector<short> samples(totalSamples);

    float phase = 0.0F;
    for (size_t i = 0; i < totalSamples; ++i)
    {
        const float t = static_cast<float>(i) / static_cast<float>(totalSamples);
        const float freq = 130.0F - 40.0F * t;
        phase += 2.0F * PI_CONST * freq / static_cast<float>(SAMPLE_RATE);

        // Mistura de onda quadrada com ruído para efeito de "impacto/estômago pesado"
        const float square = (std::sin(phase) > 0.0F ? 1.0F : -1.0F);
        const float noise = static_cast<float>((std::rand() % 2000) - 1000) / 1000.0F;
        const float env = (1.0F - t);
        const float val = (square * 0.7F + noise * 0.3F) * env;

        samples[i] = static_cast<short>(val * 20000.0F);
    }

    return CreateSoundFromSamples(samples);
}

Sound AudioManager::GenerateGameOverSound()
{
    const float duration = 0.85F;
    const size_t totalSamples = static_cast<size_t>(SAMPLE_RATE * duration);
    std::vector<short> samples(totalSamples);

    // 4 notas descendentes: E4, C4, A3, F3
    const float notes[] = {329.63F, 261.63F, 220.00F, 174.61F};
    const size_t samplesPerNote = totalSamples / 4;

    for (size_t noteIdx = 0; noteIdx < 4; ++noteIdx)
    {
        float phase = 0.0F;
        const float freq = notes[noteIdx];
        const size_t start = noteIdx * samplesPerNote;
        const size_t end = (noteIdx == 3) ? totalSamples : (start + samplesPerNote);

        for (size_t i = start; i < end; ++i)
        {
            const float t = static_cast<float>(i - start) / static_cast<float>(end - start);
            phase += 2.0F * PI_CONST * freq / static_cast<float>(SAMPLE_RATE);

            const float env = (1.0F - t * 0.7F);
            const float val = (std::sin(phase) + 0.4F * std::sin(phase * 0.5F)) * env;
            samples[i] = static_cast<short>(val * 24000.0F);
        }
    }

    return CreateSoundFromSamples(samples);
}
