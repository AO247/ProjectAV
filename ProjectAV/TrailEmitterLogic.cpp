#include "TrailEmitterLogic.h"
#include "ParticleSystemComponent.h"
#include "Node.h"

namespace dx = DirectX;

void TrailEmitterLogic::Update(float dt, ParticleSystemComponent& system)
{
    // Get the owner node's current world position
    const auto currentPosition = system.GetOwner()->GetWorldPosition();

    // On the very first frame, we don't have a previous position to compare with.
    // So, we just record the current one and exit. No trail can be generated yet.
    if (isFirstUpdate)
    {
        lastPosition = currentPosition;
        isFirstUpdate = false;
        return;
    }

    // --- Core Logic ---

    // 1. Calculate the distance the node has moved since the last frame.
    dx::XMVECTOR vecLastPos = dx::XMLoadFloat3(&lastPosition);
    dx::XMVECTOR vecCurrentPos = dx::XMLoadFloat3(&currentPosition);
    float distanceMoved = dx::XMVectorGetX(dx::XMVector3Length(dx::XMVectorSubtract(vecCurrentPos, vecLastPos)));

    // 2. Determine the distance required between each particle emission.
    //    If ParticlesPerMeter is 50, then stepDistance will be 1/50 = 0.02 meters.
    const float stepDistance = 1.0f / system.ParticlesPerMeter;

    // 3. Add the distance moved this frame to our "debt" accumulator.
    //    This variable now tracks how far we've moved since the last particle was emitted.
    distanceSinceLastEmit += distanceMoved;

    // 4. Use a while loop. As long as we have moved further than the required step distance,
    //    we need to emit a particle.
    while (distanceSinceLastEmit >= stepDistance)
    {
        // --- This is the simplest and most robust part ---
        // We don't try to calculate the exact position. We simply emit a particle at the
        // node's CURRENT position. This is simple, fast, and guaranteed to be visible.
        system.EmitParticle(currentPosition);

        // "Pay off" the debt by subtracting the distance of one step.
        // The leftover amount is carried to the next frame.
        distanceSinceLastEmit -= stepDistance;
    }

    // 5. Finally, update the last position for the next frame.
    lastPosition = currentPosition;
}