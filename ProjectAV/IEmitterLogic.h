#pragma once
#include <DirectXMath.h>

// Forward declare to avoid circular dependency
class ParticleSystemComponent;

// Abstract base class for all emitter behaviors
class IEmitterLogic
{
public:
    virtual ~IEmitterLogic() = default;

    // The logic's update function. It's given delta time and a reference
    // to the particle system it needs to control.
    virtual void Update(float dt, ParticleSystemComponent& system) = 0;
};