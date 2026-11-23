#pragma once

#include <string>
#include <vector>
#include "raylib.h"

class EffectsSystem
{
public:
    void Reset();
    void Update(float deltaTime);
    void SpawnParticles(Vector3 position, Color color, int count, float size);
    void Draw() const;
    void SpawnPopup(const char* text, Color color);
    void DrawPopups() const;

private:
    struct Particle
    {
        Vector3 position{};
        Vector3 velocity{};
        float lifetime = 0.0F;
        float maxLifetime = 0.55F;
        Color color = LIME;
        float size = 0.10F;
    };

    struct FloatingText
    {
        std::string text;
        float x = 0.0F;
        float y = 0.0F;
        float lifetime = 0.0F;
        Color color = WHITE;
    };

    std::vector<Particle> particles;
    std::vector<FloatingText> popups;
};
