#include "CircleEmitterLogic.h"
#include "ParticleSystemComponent.h"
#include "Node.h"

namespace dx = DirectX;

// A local constant for Pi
constexpr float PI = 3.14159265f;

// The constructor now initializes our random distributions.
CircleEmitterLogic::CircleEmitterLogic()
    : rng(std::random_device{}()),
    angle_dist(0.0f, 2.0f * PI), // Random angle from 0 to 360 degrees
    radius_dist(0.0f, 1.0f)      // Random value from 0 to 1 for the radius
{
}

void CircleEmitterLogic::Update(float dt, ParticleSystemComponent& system)
{
    // 1. Control the rate of emission
    timeSinceLastEmission += dt;
    const float emissionPeriod = 1.0f / ParticlesPerSecond;

    while (timeSinceLastEmission > emissionPeriod)
    {
        // 2. Generate a random angle for this particle
        const float randomAngle = angle_dist(rng);

        // 3. Determine the radius for this particle
        float spawnRadius = Radius; // Default to spawning on the edge
        if (bFill)
        {
            // If filling the disc, we need a random radius from 0 to the max.
            // We take the square root of a random number to ensure a uniform
            // distribution across the area of the disc, preventing particles
            // from clustering in the center.
            spawnRadius = Radius * sqrtf(radius_dist(rng));
        }

        // 4. Calculate the local position of the particle on the circle
        dx::XMFLOAT3 localPos = {};
        switch (Orientation)
        {
        case Plane::XY: // Upright circle facing forward
            localPos.x = spawnRadius * cosf(randomAngle);
            localPos.y = spawnRadius * sinf(randomAngle);
            localPos.z = 0.0f;
            break;

        case Plane::XZ: // Flat circle on the ground
            localPos.x = spawnRadius * cosf(randomAngle);
            localPos.y = 0.0f;
            localPos.z = spawnRadius * sinf(randomAngle);
            break;

        case Plane::YZ: // Upright circle facing sideways
            localPos.x = 0.0f;
            localPos.y = spawnRadius * cosf(randomAngle);
            localPos.z = spawnRadius * sinf(randomAngle);
            break;
        }

        // 5. Transform the local position to world space
        const auto ownerPos = system.GetOwner()->GetWorldPosition();
        dx::XMVECTOR finalPosVec = dx::XMVectorAdd(dx::XMLoadFloat3(&localPos), dx::XMLoadFloat3(&ownerPos));

        dx::XMFLOAT3 finalPos;
        dx::XMStoreFloat3(&finalPos, finalPosVec);

        // 6. Tell the system to emit a particle at this final calculated position
        system.EmitParticle(finalPos);

        // 7. Advance the state for the next particle
        timeSinceLastEmission -= emissionPeriod;
    }
}