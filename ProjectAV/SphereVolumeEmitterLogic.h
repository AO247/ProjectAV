#pragma once
#include "IEmitterLogic.h"
#include <random>

class SphereVolumeEmitterLogic : public IEmitterLogic
{
public:
    // Constructor
    SphereVolumeEmitterLogic();

    // The Update signature matches the new interface.
    void Update(float dt, ParticleSystemComponent& system) override;
    void EmitBurst(unsigned int count, ParticleSystemComponent& system) override;
public:
    // --- Public Properties ---
    float SpawnRadius = 3.0f;
    float ParticlesPerSecond = 150.0f;

private:
    float timeSinceLastEmission = 0.0f;
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist_unit;
};