#pragma once
#include "IEmitterLogic.h"

class PointEmitterLogic : public IEmitterLogic
{
public:
    void Update(float dt, ParticleSystemComponent& system) override;

private:
    float timeSinceLastEmission = 0.0f;
};