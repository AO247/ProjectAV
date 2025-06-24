#include "SphereVolumeEmitterLogic.h"
#include "ParticleSystemComponent.h"
#include "Node.h"
#include <DirectXMath.h>

namespace dx = DirectX;

// Constructor
SphereVolumeEmitterLogic::SphereVolumeEmitterLogic()
    : rng(std::random_device{}()),
    dist_unit(0.0f, 1.0f)
{
}

void SphereVolumeEmitterLogic::EmitBurst(unsigned int count, ParticleSystemComponent& system)
{
    const auto ownerWorldTransform = system.GetOwner()->GetWorldTransform();

    // Simply loop 'count' times and emit a particle each time.
    for (UINT i = 0; i < count; ++i)
    {
        // Generate a random point INSIDE the volume of a unit sphere.
        dx::XMVECTOR randomVec;
        float lengthSq;
        do {
            randomVec = dx::XMVectorSet(
                dist_unit(rng) * 2.0f - 1.0f,
                dist_unit(rng) * 2.0f - 1.0f,
                dist_unit(rng) * 2.0f - 1.0f,
                0.0f);
            lengthSq = dx::XMVectorGetX(dx::XMVector3LengthSq(randomVec));
        } while (lengthSq > 1.0f);

        dx::XMVECTOR localPosVec = dx::XMVectorScale(randomVec, SpawnRadius);
        dx::XMVECTOR worldPosVec = dx::XMVector3TransformCoord(localPosVec, ownerWorldTransform);
        dx::XMFLOAT3 finalSpawnPos;
        dx::XMStoreFloat3(&finalSpawnPos, worldPosVec);

        system.EmitParticle(finalSpawnPos);
    }
}

// The 'isFirstBurst' parameter is ignored in this version.
void SphereVolumeEmitterLogic::Update(float dt, ParticleSystemComponent& system)
{
    // 1. Control the rate of emission based on delta time.
    timeSinceLastEmission += dt;
    if (ParticlesPerSecond <= 0.0f) return;
    const float emissionPeriod = 1.0f / ParticlesPerSecond;

    // Get the owner's world transform once for efficiency.
    const auto ownerWorldTransform = system.GetOwner()->GetWorldTransform();

    // 2. Spawn particles over time.
    while (timeSinceLastEmission > emissionPeriod)
    {
        // Generate a random point INSIDE the volume of a unit sphere.
        dx::XMVECTOR randomVec;
        float lengthSq;
        do {
            randomVec = dx::XMVectorSet(
                dist_unit(rng) * 2.0f - 1.0f,
                dist_unit(rng) * 2.0f - 1.0f,
                dist_unit(rng) * 2.0f - 1.0f,
                0.0f);
            lengthSq = dx::XMVectorGetX(dx::XMVector3LengthSq(randomVec));
        } while (lengthSq > 1.0f);

        // Scale this unit-space position by the desired SpawnRadius.
        dx::XMVECTOR localPosVec = dx::XMVectorScale(randomVec, SpawnRadius);

        // Transform this local position into world space for the spawn location.
        dx::XMVECTOR worldPosVec = dx::XMVector3TransformCoord(localPosVec, ownerWorldTransform);
        dx::XMFLOAT3 finalSpawnPos;
        dx::XMStoreFloat3(&finalSpawnPos, worldPosVec);

        // Emit the particle. This will use the default velocity from the main component.
        system.EmitParticle(finalSpawnPos);

        // Decrement time for the next particle.
        timeSinceLastEmission -= emissionPeriod;
    }
}