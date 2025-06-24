// SphereToCenterEmitterLogic.h

#pragma once
#include "IEmitterLogic.h"
#include <random>

class SphereToCenterEmitterLogic : public IEmitterLogic
{
public:
    // Constructor to initialize random number generators
    SphereToCenterEmitterLogic();

    void Update(float dt, ParticleSystemComponent& system) override;

public:
    // --- Public Properties for this Emitter ---

    // The radius of the sphere on which particles will spawn.
    float SpawnRadius = 5.0f;

    // The number of particles to spawn per second.
    float ParticlesPerSecond = 200.0f;

    // The base speed at which particles travel towards the center.
    float TravelSpeed = 2.5f;

    float SpeedRandomness = 0.2f;
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
private:
    float timeSinceLastEmission = 0.0f;

    // For generating random points on the surface of a sphere

};