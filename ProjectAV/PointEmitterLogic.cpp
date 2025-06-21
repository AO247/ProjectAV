#include "PointEmitterLogic.h"
#include "ParticleSystemComponent.h"
#include "Node.h"

void PointEmitterLogic::Update(float dt, ParticleSystemComponent& system)
{
    // This is the logic moved from your old ParticleSystemComponent::Update
    timeSinceLastEmission += dt;

    // Use the public properties from the main component
    const float emissionPeriod = 1.0f / system.EmissionRate;

    while (timeSinceLastEmission > emissionPeriod)
    {
        // Get the owner node's world position
        const auto ownerPos = system.GetOwner()->GetWorldPosition();

        // Tell the system to emit one particle at that position
        system.EmitParticle(ownerPos);

        timeSinceLastEmission -= emissionPeriod;
    }
}