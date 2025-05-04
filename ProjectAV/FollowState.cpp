#include "FollowState.h"
#include "StateMachine.h"
#include "Node.h" // Include Node to get other components
#include <DirectXMath.h> // For DirectX::XMFLOAT3 operations
#include <SimpleMath.h>


#include <Windows.h> // For OutputDebugStringA
namespace dx = DirectX;
namespace sm = DirectX::SimpleMath;
FollowState::FollowState(StateMachine* pOwner) : State()
{
	pOwner->GetOwner();
}


void FollowState::Enter(StateMachine* pOwner)
{
	OutputDebugStringA("Entering FOLLOW State\n");
	time = 0.0f;

}

void FollowState::Update(StateMachine* pOwner, float dt)
{
    dx::XMFLOAT3 ownerPos = pOwner->GetOwner()->GetWorldPosition();
    dx::XMFLOAT3 playerPos = pOwner->pPlayer->GetWorldPosition();

	sm::Vector3 vOwner(ownerPos.x, ownerPos.y, ownerPos.z);
	sm::Vector3 vPlayer(playerPos.x, playerPos.y, playerPos.z);

    if (vOwner.Distance(vOwner, vPlayer) > pOwner->followDistance) // Example threshold for "in range"
    {
        pOwner->RequestStateChange(StateType::IDLE);
        return;
    }
    if (vOwner.Distance(vOwner, vPlayer) < pOwner->attackRange)
    {
		pOwner->RequestStateChange(StateType::ATTACK);
        return;
    }
    if (pOwner->pMovementComponent != nullptr) {
        pOwner->pMovementComponent->Follow(playerPos);
    }

}

void FollowState::Exit(StateMachine* pOwner)
{
	OutputDebugStringA("Exiting FOLLOW State\n");
}