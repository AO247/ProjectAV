#include "IdleState.h"
#include "StateMachine.h"
#include "Node.h" // Include Node to get other components

#include <Windows.h> // For OutputDebugStringA
#include <DirectXMath.h> // For DirectX::XMFLOAT3 operations
#include "CMath.h"

namespace dx = DirectX;
namespace sm = DirectX::SimpleMath;
IdleState::IdleState(StateMachine* pOwner) : State()
{
}

void IdleState::Enter(StateMachine* pOwner)
{
    OutputDebugStringA("Entering IDLE State\n");
    time = 0.0f;
    wanderAngle = (((float)rand() / RAND_MAX) * 2.0f * PI) - PI;
}
void IdleState::Update(StateMachine* pOwner, float dt)
{
    sm::Vector3 vOwner(pOwner->GetOwner()->GetWorldPosition());
    sm::Vector3 vPlayer(pOwner->pPlayer->GetWorldPosition());

    if (vOwner.Distance(vOwner, vPlayer) < pOwner->followDistance)
    {
        pOwner->RequestStateChange(StateType::FOLLOW);
        return;
    }




    sm::Vector3 circleCenter = pOwner->GetOwner()->GetWorldPosition();
    circleCenter += pOwner->GetOwner()->Forward() * wanderCenterDistance; // Use StateMachine's member


    float angleChangeThisFrame = (((float)rand() / RAND_MAX) - 0.5f) * 2.0f * wanderAngleChange * dt;
    wanderAngle += angleChangeThisFrame;

    sm::Vector3 displacement(cosf(wanderAngle), 0.0f, sinf(wanderAngle));
    displacement *= wanderRradius;

    dx::XMMATRIX agentYawRotation = dx::XMMatrixRotationY(pOwner->GetOwner()->GetLocalRotationEuler().y);
    dx::XMVECTOR displacementVecDX = dx::XMLoadFloat3(&displacement);
    displacementVecDX = dx::XMVector3TransformNormal(displacementVecDX, agentYawRotation);
    sm::Vector3 worldDisplacement;
    dx::XMStoreFloat3(&worldDisplacement, displacementVecDX);

    sm::Vector3 wanderTarget = circleCenter + worldDisplacement;

    pOwner->pMovementComponent->Follow(wanderTarget, 1.2f);
    pOwner->pos = wanderTarget;
    pOwner->cen = circleCenter;

}

void IdleState::Exit(StateMachine* pOwner)
{
    OutputDebugStringA("Exiting IDLE State\n");
}