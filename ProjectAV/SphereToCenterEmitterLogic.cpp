// SphereToCenterEmitterLogic.cpp

#include "SphereToCenterEmitterLogic.h"
#include "ParticleSystemComponent.h"
#include "Node.h"
#include <DirectXMath.h>

namespace dx = DirectX;

// Constructor
SphereToCenterEmitterLogic::SphereToCenterEmitterLogic()
// The distribution now provides a random number between -1.0 and 1.0,
// which we will use to scale our randomness factor.
    : rng(std::random_device{}()),
    dist(-1.0f, 1.0f)
{
}

void SphereToCenterEmitterLogic::Update(float dt, ParticleSystemComponent& system)
{
    // 1. Control the rate of emission
    timeSinceLastEmission += dt;
    if (ParticlesPerSecond <= 0.0f) return;
    const float emissionPeriod = 1.0f / ParticlesPerSecond;

    // Get the owner's world transform once for efficiency
    const auto ownerWorldTransform = system.GetOwner()->GetWorldTransform();

    while (timeSinceLastEmission > emissionPeriod)
    {
        // 2. Generate a random spawn position on the surface of a local-space sphere.
        dx::XMVECTOR randomPosVec;
        float lengthSq;
        do {
            randomPosVec = dx::XMVectorSet(dist(rng), dist(rng), dist(rng), 0.0f);
            lengthSq = dx::XMVectorGetX(dx::XMVector3LengthSq(randomPosVec));
        } while (lengthSq > 1.0f);

        dx::XMVECTOR localPosVec = dx::XMVectorScale(dx::XMVector3Normalize(randomPosVec), SpawnRadius);


        // 3. Transform this local position into world space for the spawn location.
        dx::XMVECTOR worldPosVec = dx::XMVector3TransformCoord(localPosVec, ownerWorldTransform);
        dx::XMFLOAT3 finalSpawnPos;
        dx::XMStoreFloat3(&finalSpawnPos, worldPosVec);


        // 4. Calculate the particle's initial velocity.
        // The direction is ALWAYS towards the center (negative of the local position vector).
        dx::XMVECTOR localVelocityDir = dx::XMVectorScale(dx::XMVector3Normalize(localPosVec), -1.0f);

        // +++ NEW: Add randomness to the TRAVEL SPEED +++
        // Calculate a random speed variance.
        // For example, if TravelSpeed is 2.5 and SpeedRandomness is 0.5,
        // the random part will be between -1.25 and +1.25.
        const float speedVariance = TravelSpeed * SpeedRandomness * dist(rng);
        const float finalSpeed = TravelSpeed + speedVariance;


        // 5. Transform the direction and apply the final, randomized speed.
        // Transform the local velocity direction into world space.
        dx::XMVECTOR worldVelocityDir = dx::XMVector3TransformNormal(localVelocityDir, ownerWorldTransform);

        // Set the final velocity vector using the randomized speed.
        dx::XMFLOAT3 finalVelocity;
        dx::XMStoreFloat3(
            &finalVelocity,
            dx::XMVectorScale(worldVelocityDir, finalSpeed)
        );


        // 6. Override the properties on the main component.
        system.ParticleVelocity = finalVelocity;
        system.ParticleVelocityVariance = { 0.0f, 0.0f, 0.0f };


        // 7. Emit the particle.
        system.EmitParticle(finalSpawnPos);


        // 8. Decrement time for the next particle
        timeSinceLastEmission -= emissionPeriod;
    }
}