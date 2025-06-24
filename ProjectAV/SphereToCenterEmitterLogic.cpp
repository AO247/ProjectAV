#include "SphereToCenterEmitterLogic.h"
#include "ParticleSystemComponent.h"
#include "Node.h"
#include <DirectXMath.h>

namespace dx = DirectX;

// Constructor
SphereToCenterEmitterLogic::SphereToCenterEmitterLogic()
// Initialize the random distribution to produce values between 0.0 and 1.0.
// This will be used for calculating the positive speed variance.
// For directions, we will remap this range to [-1.0, 1.0].
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
        // Map the [0,1] distribution to the [-1,1] range needed for a direction vector.
        randomPosVec = dx::XMVectorSet(
            emitter->dist(emitter->rng) * 2.0f - 1.0f,
            emitter->dist(emitter->rng) * 2.0f - 1.0f,
            emitter->dist(emitter->rng) * 2.0f - 1.0f,
            0.0f
        );
        lengthSq = dx::XMVectorGetX(dx::XMVector3LengthSq(randomPosVec));
    } while (lengthSq > 1.0f); // Ensure uniform distribution on the sphere

    dx::XMVECTOR localPosVec = dx::XMVectorScale(dx::XMVector3Normalize(randomPosVec), emitter->SpawnRadius);
    dx::XMVECTOR worldPosVec = dx::XMVector3TransformCoord(localPosVec, ownerWorldTransform);
    dx::XMFLOAT3 finalSpawnPos;
    dx::XMStoreFloat3(&finalSpawnPos, worldPosVec);

    // --- Calculate the particle's initial velocity ---
    // Direction is always towards the center.
    dx::XMVECTOR localVelocityDir = dx::XMVectorScale(dx::XMVector3Normalize(localPosVec), -1.0f);

    // Calculate a random speed variance. Since dist(rng) is [0, 1], this is always positive.
    const float speedVariance = emitter->SpeedRandomness * emitter->dist(emitter->rng);
    const float finalSpeed = emitter->TravelSpeed + speedVariance;

    // Transform direction to world space and apply final speed.
    dx::XMVECTOR worldVelocityDir = dx::XMVector3TransformNormal(localVelocityDir, ownerWorldTransform);
    dx::XMFLOAT3 finalVelocity;
    dx::XMStoreFloat3(&finalVelocity, dx::XMVectorScale(worldVelocityDir, finalSpeed));

    // --- Override properties and emit ---
    system.ParticleVelocity = finalVelocity;
    system.ParticleVelocityVariance = { 0.0f, 0.0f, 0.0f }; // We handle all randomness here
    system.EmitParticle(finalSpawnPos);
}

// For continuous, timed emission (Looping effects or non-burst One-Shots)
void SphereToCenterEmitterLogic::Update(float dt, ParticleSystemComponent& system)
{
    // For this emitter, a one-shot burst is handled entirely by EmitBurst.
    // The regular Update should only handle timed emission.
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