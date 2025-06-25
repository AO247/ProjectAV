// VortexEmitterLogic.h

#pragma once
#include "IEmitterLogic.h"
#include <random>
#include <DirectXMath.h>

class VortexEmitterLogic : public IEmitterLogic
{
public:
    VortexEmitterLogic();

    // For continuous, timed emission
    void Update(float dt, ParticleSystemComponent& system) override;

    // For instantaneous, count-based emission
    void EmitBurst(unsigned int count, ParticleSystemComponent& system) override;

public:
    // --- Public Properties for the Vortex Emitter ---

    // The radius of the circular path.
    float Radius = 3.0f;

    // The base speed at which particles orbit, in radians per second.
    // A value of 6.28 (2*PI) means one full circle per second.
    float BaseOrbitalSpeed = 2.0f;

    // A random value from 0 to this variance will be ADDED to the base speed.
    float OrbitalSpeedVariance = 1.0f;

    // If true, the plane of the vortex will be randomized each time Play() is called.
    // If false, it will use the default XZ plane (a flat circle).
    bool bRandomizePlane = true;

    // The number of particles to spawn per second for timed emission.
    float ParticlesPerSecond = 100.0f;

private:
    // Helper function to emit a single particle with the vortex logic.
    void EmitSingleParticle(ParticleSystemComponent& system, const DirectX::XMMATRIX& ownerWorldTransform);

    // Helper to generate a new random orbital plane.
    void GenerateNewPlane();

private:
    float timeSinceLastEmission = 0.0f;
    float currentAngle = 0.0f; // Tracks the position along the circle for new particles

    // Random number generation
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;

    // Basis vectors that define the plane of the circle.
    // All particle positions and velocities are calculated relative to these.
    DirectX::XMFLOAT3 planeBasisX = { 1.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 planeBasisY = { 0.0f, 0.0f, 1.0f };
};