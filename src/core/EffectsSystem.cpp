#include "EffectsSystem.h"

#include <algorithm>
#include "core/Settings.h"
#include "ui/HUD.h"

void EffectsSystem::Reset()
{
    particles.clear();
    popups.clear();
    particles.reserve(128);
    popups.reserve(16);
}

void EffectsSystem::Update(float deltaTime)
{
    for (Particle& particle : particles)
    {
        particle.position.x += particle.velocity.x * deltaTime;
        particle.position.y += particle.velocity.y * deltaTime;
        particle.position.z += particle.velocity.z * deltaTime;
        particle.velocity.y -= Settings::GRAVITY * deltaTime * 0.35F;
        particle.lifetime -= deltaTime;
    }
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
                       [](const Particle& particle) { return particle.lifetime <= 0.0F; }),
        particles.end());

    for (FloatingText& popup : popups)
    {
        popup.y -= 45.0F * deltaTime;
        popup.lifetime -= deltaTime;
    }
    popups.erase(
        std::remove_if(popups.begin(), popups.end(),
                       [](const FloatingText& popup) { return popup.lifetime <= 0.0F; }),
        popups.end());
}

void EffectsSystem::SpawnParticles(Vector3 position, Color color, int count, float size)
{
    for (int i = 0; i < count; ++i)
    {
        if (particles.size() >= 128)
        {
            break;
        }
        const float horizontal = static_cast<float>(GetRandomValue(-10, 10)) * 0.22F;
        const float vertical = static_cast<float>(GetRandomValue(3, 9)) * 0.40F;
        const float depth = static_cast<float>(GetRandomValue(-10, 10)) * 0.18F;
        particles.push_back({
            position,
            {horizontal, vertical, depth},
            Settings::PARTICLE_LIFETIME,
            Settings::PARTICLE_LIFETIME,
            color,
            size});
    }
}

void EffectsSystem::Draw() const
{
    for (const Particle& particle : particles)
    {
        const float alpha = std::clamp(
            particle.lifetime / particle.maxLifetime, 0.0F, 1.0F);
        DrawSphere(particle.position, particle.size, Fade(particle.color, alpha));
    }
}

void EffectsSystem::SpawnPopup(const char* text, Color color)
{
    popups.push_back({
        std::string(text),
        static_cast<float>(Settings::WINDOW_WIDTH) * 0.5F,
        static_cast<float>(Settings::WINDOW_HEIGHT) * 0.45F,
        1.1F,
        color});
}

void EffectsSystem::DrawPopups() const
{
    for (const FloatingText& popup : popups)
    {
        const float alpha = std::clamp(popup.lifetime / 1.1F, 0.0F, 1.0F);
        DrawFloatingMessage(popup.text.c_str(), popup.x, popup.y, 22.0F,
                            popup.color, alpha);
    }
}
