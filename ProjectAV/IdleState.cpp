#include "IdleState.h"
#include "StateMachine.h"
#include "Node.h" // Include Node to get other components

#include <Windows.h> // For OutputDebugStringA
#include <DirectXMath.h> // For DirectX::XMFLOAT3 operations

namespace dx = DirectX;
namespace sm = DirectX::SimpleMath;
IdleState::IdleState(StateMachine* pOwner) : State()
{
}

void IdleState::Enter(StateMachine* pOwner)
{
    OutputDebugStringA("Entering IDLE State\n");
    time = 0.0f;
    previousPos = pOwner->GetOwner()->GetWorldPosition();
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

    sm::Vector3 center(pOwner->GetOwner()->GetWorldPosition());
	sm::Vector3 forward(pOwner->GetOwner()->Forward());
    center += forward * 3.0f;
    sm::Vector3 newPos = center + previousPos * radius;
    int randIx = (rand() % 3) - 1;
    int randIz = (rand() % 3) - 1;

	newPos.x += randIx / 5.0f;
    newPos.z += randIz / 5.0f;

    sm::Vector3 dir = newPos - center;
    dir.Normalize();
    newPos = center + dir * radius;
    pOwner->pMovementComponent->Follow(newPos, 2.0f);
	pOwner->pos = newPos;
    pOwner->cen = center;
	previousPos = dir;

}

void IdleState::Exit(StateMachine* pOwner)
{
    OutputDebugStringA("Exiting IDLE State\n");
}