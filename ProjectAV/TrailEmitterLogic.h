#pragma once
#include "IEmitterLogic.h"

class TrailEmitterLogic : public IEmitterLogic
{
public:
    void Update(float dt, ParticleSystemComponent& system) override;

private:
    DirectX::XMFLOAT3 lastPosition = { 0.0f, 0.0f, 0.0f };
    bool isFirstUpdate = true;

    // This tracks how far the node has moved since the last particle was laid down.
    float distanceSinceLastEmit = 0.0f;
};