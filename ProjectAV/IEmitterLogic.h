// In IEmitterLogic.h
#pragma once
#include <DirectXMath.h>

class ParticleSystemComponent;

class IEmitterLogic
{
public:
    virtual ~IEmitterLogic() = default;

    // For continuous, timed emission (Looping effects or non-burst One-Shots)
    virtual void Update(float dt, ParticleSystemComponent& system) = 0;

    // +++ ADD NEW BURST FUNCTION +++
    // For instantaneous, count-based emission (Burst One-Shots)
    // The default implementation does nothing, so not all emitters need to support it.
    virtual void EmitBurst(unsigned int count, ParticleSystemComponent& system) {}
};