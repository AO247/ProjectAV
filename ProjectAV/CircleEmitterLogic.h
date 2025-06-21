#pragma once
#include "IEmitterLogic.h"
#include <DirectXMath.h>
#include <random> // We'll need this for the random angle

class CircleEmitterLogic : public IEmitterLogic
{
public:
    // Constructor to initialize the random number generator
    CircleEmitterLogic();

    // Enum to easily define the circle's orientation in 3D space.
    enum class Plane
    {
        XY, // Standing up, facing the camera in a standard view
        XZ, // Flat on the ground
        YZ  // Standing up, facing sideways
    };

public:
    void Update(float dt, ParticleSystemComponent& system) override;

    // --- Public Properties for Circle Emission ---

    // The radius of the circle in world units.
    float Radius = 1.0f;
    // How many particles to spawn per second. This logic uses its own
    // rate and ignores the EmissionRate on the ParticleSystemComponent.
    float ParticlesPerSecond = 100.0f;
    // The plane in which the circle lies, relative to the owner Node's orientation.
    Plane Orientation = Plane::XZ;

    // NEW: If true, particles will spawn randomly within the circle's area (a disc).
    // If false, they will spawn randomly only on the circle's edge (a ring).
    bool bFill = false;

private:
    float timeSinceLastEmission = 0.0f;

    // Private random number generation members
    std::mt19937 rng;
    std::uniform_real_distribution<float> angle_dist;
    std::uniform_real_distribution<float> radius_dist;
};