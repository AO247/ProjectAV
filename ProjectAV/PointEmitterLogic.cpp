#include "PointEmitterLogic.h"
#include "ParticleSystemComponent.h"
#include "Node.h"

// The timed Update function for looping effects remains the same.
void PointEmitterLogic::Update(float dt, ParticleSystemComponent& system)
{
    // This logic is only for continuous/timed emission.
    timeSinceLastEmission += dt;

    if (system.EmissionRate <= 0.0f) return; // Prevent division by zero
    const float emissionPeriod = 1.0f / system.EmissionRate;

    while (timeSinceLastEmission > emissionPeriod)
    {
        // Get the owner node's world position.
        const auto ownerPos = system.GetOwner()->GetWorldPosition();

        // Tell the system to emit one particle at that position.
        system.EmitParticle(ownerPos);

        timeSinceLastEmission -= emissionPeriod;
    }
}


// +++ ADD THE NEW BURST FUNCTION IMPLEMENTATION +++
void PointEmitterLogic::EmitBurst(unsigned int count, ParticleSystemComponent& system)
{
    // Get the single spawn position once for efficiency.
    const auto ownerPos = system.GetOwner()->GetWorldPosition();

    // Loop 'count' times and emit a particle at the same position each time.
    for (unsigned int i = 0; i < count; ++i)
    {
        system.EmitParticle(ownerPos);
    }
}