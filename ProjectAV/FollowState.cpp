#include "FollowState.h"
#include "StateMachine.h"
#include "Node.h" 
#include <DirectXMath.h> 
#include <SimpleMath.h>


#include "Win.h"
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
    if (pOwner->attackComponents.size() > 0) {
        int randI = rand() % pOwner->attackComponents.size();
        pOwner->pAttackComponent = pOwner->attackComponents[randI];
        pOwner->attackRange = pOwner->pAttackComponent->attackRange;
    }

}

void FollowState::Update(StateMachine* pOwner, float dt)
{
    dx::XMFLOAT3 ownerPos = pOwner->GetOwner()->GetWorldPosition();
    dx::XMFLOAT3 playerPos = pOwner->pPlayer->GetWorldPosition();

	sm::Vector3 vOwner(ownerPos.x, ownerPos.y, ownerPos.z);
	sm::Vector3 vPlayer(playerPos.x, playerPos.y, playerPos.z);

    if (vOwner.Distance(vOwner, vPlayer) > pOwner->followDistance) 
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
        pOwner->pMovementComponent->Follow(dt, playerPos);
    }

}

void FollowState::Exit(StateMachine* pOwner)
{
	OutputDebugStringA("Exiting FOLLOW State\n");
}