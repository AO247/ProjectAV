#include "FollowState.h"
#include "StateMachine.h"
#include "Node.h" 
#include <DirectXMath.h> 
#include <SimpleMath.h>
#include "PhysicsCommon.h"

#include "Win.h"
namespace dx = DirectX;
namespace sm = DirectX::SimpleMath;
FollowState::FollowState(StateMachine* pOwner) : State()
{
	pOwner->GetOwner();
}


void FollowState::Enter(StateMachine* pOwner)
{
	//OutputDebugStringA("Entering FOLLOW State\n");
	time = 0.0f;
    if (pOwner->attackComponents.size() > 0) {
        int randI = rand() % pOwner->attackComponents.size();
        pOwner->pAttackComponent = pOwner->attackComponents[randI];
        pOwner->attackRange = pOwner->pAttackComponent->attackRange;
    }

}

void FollowState::Update(StateMachine* pOwner, float dt)
{
    sm::Vector3 ownerPos = pOwner->GetOwner()->GetWorldPosition();
    sm::Vector3 playerPos = pOwner->pPlayer->GetWorldPosition();

    if (ownerPos.Distance(ownerPos, playerPos) > pOwner->followDistance)
    {
        pOwner->RequestStateChange(StateType::IDLE);
        return;
    }
    if (ownerPos.Distance(ownerPos, playerPos) < pOwner->attackRange)
    {
        sm::Vector3 temporaryDirection = playerPos - ownerPos;
		float length = temporaryDirection.Length();
        length -= 2.0f;
        temporaryDirection.Normalize();
        RRayCast ray1 = RRayCast(
            RVec3(ownerPos.x, ownerPos.y, ownerPos.z),
            RVec3(temporaryDirection.x, temporaryDirection.y, temporaryDirection.z) * length
        );
        RayCastResult result1;
        if (!PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(ray1, result1, IgnoreMultipleBroadPhaseLayerFilter({ BroadPhaseLayers::ENEMY, BroadPhaseLayers::TRIGGER }),
            IgnoreMultipleObjectLayerFilter({ Layers::ENEMY, Layers::TRIGGER })))
        {
            pOwner->RequestStateChange(StateType::ATTACK);
            return;
        }
    }
    if (pOwner->pMovementComponent != nullptr) {
        pOwner->pMovementComponent->Follow(dt, playerPos);
    }

}

void FollowState::Exit(StateMachine* pOwner)
{
	//OutputDebugStringA("Exiting FOLLOW State\n");
}