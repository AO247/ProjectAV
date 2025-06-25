// VortexEmitterLogic.cpp

#include "VortexEmitterLogic.h"
#include "ParticleSystemComponent.h"
#include "Node.h"

namespace dx = DirectX;

VortexEmitterLogic::VortexEmitterLogic()
    : rng(std::random_device{}()),
    dist(0.0f, 1.0f)
{
    GenerateNewPlane();
}

// Private helper to generate a new random orbital plane
void VortexEmitterLogic::GenerateNewPlane()
{
    if (bRandomizePlane)
    {
        // 1. Generate a random normalized vector to serve as the plane's normal (our new Z-axis).
        dx::XMVECTOR randomNormal;
        float lengthSq;
        do {
            randomNormal = dx::XMVectorSet(
                dist(rng) * 2.0f - 1.0f,
                dist(rng) * 2.0f - 1.0f,
                dist(rng) * 2.0f - 1.0f,
                0.0f);
            lengthSq = dx::XMVectorGetX(dx::XMVector3LengthSq(randomNormal));
        } while (lengthSq < 0.001f); // Avoid zero vector
        randomNormal = dx::XMVector3Normalize(randomNormal);

        // 2. Create a temporary "up" vector. If our normal is too close to the world up,
        // use a different axis to avoid issues with the cross product.
        dx::XMVECTOR tempUp = dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        if (dx::XMVectorGetX(dx::XMVector3Dot(randomNormal, tempUp)) > 0.99f)
        {
            tempUp = dx::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f); // Use X-axis instead
        }

        // 3. Use cross products to generate two perpendicular basis vectors from the normal.
        dx::XMVECTOR basisX = dx::XMVector3Normalize(dx::XMVector3Cross(tempUp, randomNormal));
        dx::XMVECTOR basisY = dx::XMVector3Normalize(dx::XMVector3Cross(randomNormal, basisX));

        dx::XMStoreFloat3(&planeBasisX, basisX);
        dx::XMStoreFloat3(&planeBasisY, basisY);
    }
    else
    {
        // Use the default flat XZ plane
        planeBasisX = { 1.0f, 0.0f, 0.0f };
        planeBasisY = { 0.0f, 0.0f, 1.0f };
    }
}

// Helper to emit one particle
void VortexEmitterLogic::EmitSingleParticle(ParticleSystemComponent& system, const DirectX::XMMATRIX& ownerWorldTransform)
{
    // --- 1. Calculate Position in World Space ---
    dx::XMVECTOR basisX = dx::XMLoadFloat3(&planeBasisX);
    dx::XMVECTOR basisY = dx::XMLoadFloat3(&planeBasisY);

    // Calculate the point on the circle's edge in LOCAL space
    dx::XMVECTOR localPosVec = dx::XMVectorScale(basisX, cosf(currentAngle) * Radius);
    localPosVec = dx::XMVectorAdd(localPosVec, dx::XMVectorScale(basisY, sinf(currentAngle) * Radius));

    // +++ FIX: Transform the local position to WORLD space +++
    // This respects the node's position, rotation, and scale.
    dx::XMVECTOR worldPosVec = dx::XMVector3TransformCoord(localPosVec, ownerWorldTransform);

    dx::XMFLOAT3 finalSpawnPos;
    dx::XMStoreFloat3(&finalSpawnPos, worldPosVec);

    // --- 2. Calculate Velocity in World Space ---
    // The velocity is the tangent of the circle at the spawn point.
    dx::XMVECTOR localVelDir = dx::XMVectorScale(basisX, -sinf(currentAngle));
    localVelDir = dx::XMVectorAdd(localVelDir, dx::XMVectorScale(basisY, cosf(currentAngle)));

    // Apply randomized speed
    const float speedVariance = BaseOrbitalSpeed * OrbitalSpeedVariance * dist(rng);
    const float finalSpeed = BaseOrbitalSpeed + speedVariance;

    // Transform the final LOCAL velocity direction into WORLD space.
    // Use TransformNormal because velocity is a direction.
    dx::XMVECTOR worldVelDir = dx::XMVector3TransformNormal(localVelDir, ownerWorldTransform);

    dx::XMFLOAT3 finalVelocity;
    dx::XMStoreFloat3(&finalVelocity, dx::XMVectorScale(worldVelDir, finalSpeed));

    // --- 3. Override properties and emit ---
    // Override the particle system's velocity properties for this particle.
    system.ParticleVelocity = finalVelocity;
    // Set variance to zero because we have already calculated the final velocity.
    system.ParticleVelocityVariance = { 0.0f, 0.0f, 0.0f };

    // +++ FIX: Pass the CALCULATED world position to EmitParticle +++
    system.EmitParticle(finalSpawnPos);
}

// For timed emission
void VortexEmitterLogic::Update(float dt, ParticleSystemComponent& system)
{
    const auto ownerWorldTransform = system.GetOwner()->GetWorldTransform();

    timeSinceLastEmission += dt;
    if (ParticlesPerSecond <= 0.0f) return;
    const float emissionPeriod = 1.0f / ParticlesPerSecond;

    currentAngle += BaseOrbitalSpeed * dt;

    while (timeSinceLastEmission > emissionPeriod)
    {
        EmitSingleParticle(system, ownerWorldTransform);
        timeSinceLastEmission -= emissionPeriod;
    }
}

// For burst emission
void VortexEmitterLogic::EmitBurst(unsigned int count, ParticleSystemComponent& system)
{
    GenerateNewPlane();
    currentAngle = 0.0f;
    const float angleStep = (2.0f * 3.14159265f) / count;

    const auto ownerWorldTransform = system.GetOwner()->GetWorldTransform();

    for (unsigned int i = 0; i < count; ++i)
    {
        EmitSingleParticle(system, ownerWorldTransform);
        currentAngle += angleStep;
    }
}
