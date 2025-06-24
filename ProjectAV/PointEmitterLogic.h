#pragma once
#include "IEmitterLogic.h"

class PointEmitterLogic : public IEmitterLogic
{
public:
    // For continuous, timed emission
    void Update(float dt, ParticleSystemComponent& system) override;

    // +++ ADD THE BURST FUNCTION DECLARATION +++
    // For instantaneous, count-based emission
    void EmitBurst(unsigned int count, ParticleSystemComponent& system) override;

private:
    float timeSinceLastEmission = 0.0f;
};