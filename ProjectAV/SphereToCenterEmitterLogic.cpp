// In SphereToCenterEmitterLogic.cpp

#include "SphereToCenterEmitterLogic.h"
#include "ParticleSystemComponent.h"
#include "Node.h"
#include <DirectXMath.h>

namespace dx = DirectX;

// Constructor
SphereToCenterEmitterLogic::SphereToCenterEmitterLogic()
    : rng(std::random_device{}()),
    dist(0.0f, 1.0f)
{
}

// Private helper function to emit one particle.
// This avoids duplicating the complex logic in both Update and EmitBurst.
void EmitSingleParticle(SphereToCenterEmitterLogic* emitter, ParticleSystemComponent& system, const DirectX::XMMATRIX& ownerWorldTransform)
{
    // --- Generate a random spawn position on the surface of a local-space sphere ---
    dx::XMVECTOR randomPosVec;
    float lengthSq;
    do {
        randomPosVec = dx::XMVectorSet(
            emitter->dist(emitter->rng) * 2.0f - 1.0f,
            emitter->dist(emitter->rng) * 2.0f - 1.0f,
            emitter->dist(emitter->rng) * 2.0f - 1.0f,
            0.0f
        );
        lengthSq = dx::XMVectorGetX(dx::XMVector3LengthSq(randomPosVec));
    } while (lengthSq > 1.0f);

    dx::XMVECTOR localPosVec = dx::XMVectorScale(dx::XMVector3Normalize(randomPosVec), emitter->SpawnRadius);
    dx::XMVECTOR worldPosVec = dx::XMVector3TransformCoord(localPosVec, ownerWorldTransform);
    dx::XMFLOAT3 finalSpawnPos;
    dx::XMStoreFloat3(&finalSpawnPos, worldPosVec);

    // --- Calculate the particle's initial velocity ---
    dx::XMVECTOR localVelocityDir = dx::XMVectorScale(dx::XMVector3Normalize(localPosVec), -1.0f);
    const float speedVariance = emitter->SpeedRandomness * emitter->dist(emitter->rng);
    const float finalSpeed = emitter->TravelSpeed + speedVariance;
    dx::XMVECTOR worldVelocityDir = dx::XMVector3TransformNormal(localVelocityDir, ownerWorldTransform);

    dx::XMFLOAT3 finalVelocity;
    dx::XMStoreFloat3(&finalVelocity, dx::XMVectorScale(worldVelocityDir, finalSpeed));

    // --- DELETED ---
    // system.ParticleVelocity = finalVelocity;
    // system.ParticleVelocityVariance = { 0.0f, 0.0f, 0.0f };
    // system.EmitParticle(finalSpawnPos);

    // +++ ADDED: Call the new overload with the calculated world-space values +++
    system.EmitParticle(finalSpawnPos, finalVelocity);
}

// For continuous, timed emission (Looping effects or non-burst One-Shots)
void SphereToCenterEmitterLogic::Update(float dt, ParticleSystemComponent& system)
{
    timeSinceLastEmission += dt;
    if (ParticlesPerSecond <= 0.0f) return;
    const float emissionPeriod = 1.0f / ParticlesPerSecond;

    const auto ownerWorldTransform = system.GetOwner()->GetWorldTransform();

    while (timeSinceLastEmission > emissionPeriod)
    {
        EmitSingleParticle(this, system, ownerWorldTransform);
        timeSinceLastEmission -= emissionPeriod;
    }
}