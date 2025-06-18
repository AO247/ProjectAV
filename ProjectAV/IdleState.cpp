#include "IdleState.h"
#include "StateMachine.h"
#include "Node.h" 

#include <Windows.h> 
#include <DirectXMath.h> 
#include "CMath.h"

namespace dx = DirectX;
namespace sm = DirectX::SimpleMath;
IdleState::IdleState(StateMachine* pOwner) : State()
{
}

void IdleState::Enter(StateMachine* pOwner)
{
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
    circleCenter += pOwner->GetOwner()->Forward() * wanderCenterDistance; 


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

    pOwner->pMovementComponent->Follow(dt, wanderTarget, 1.4f);
    pOwner->pos = wanderTarget;
    pOwner->cen = circleCenter;

}

void IdleState::Exit(StateMachine* pOwner)
{
}