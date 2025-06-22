#include "AttackState.h"
#include "StateMachine.h"
#include "Node.h" 
#include "EnemyConfig.h"
#include "AnimationComponent.h"

#include "Win.h" 

AttackState::AttackState(StateMachine* pOwner) : State()
{
}


void AttackState::Enter(StateMachine* pOwner)
{
	//OutputDebugStringA("Entering ATTACK State\n");
	if (pOwner->pAttackComponent != nullptr) {
		time = 0.0f;
		sm::Vector3 facingDirection = sm::Vector3(pOwner->pPlayer->GetWorldPosition())
			- sm::Vector3(pOwner->GetOwner()->GetWorldPosition());
		facingDirection.Normalize();

		float targetYaw = atan2f(facingDirection.x, facingDirection.z);

		pOwner->GetOwner()->SetLocalRotation({ 0.0f, targetYaw, 0.0f });

	}

	AnimationComponent* animComp = pOwner->GetOwnerNode()->GetComponent<AnimationComponent>();
	if (animComp) {
		switch (pOwner->enemyType)
		{
		case EnemyType::BASIC:
			int randomAttack = std::rand() % 2;  
			if (randomAttack == 0)
				animComp->PlayAnimation(EnemyAnimationIndices::BASIC_ATTACK1);
			else
				animComp->PlayAnimation(EnemyAnimationIndices::BASIC_ATTACK2);
			break;
		}
	}

}

void AttackState::Update(StateMachine* pOwner, float dt)
{
	pOwner->pAttackComponent->Attack(dt);
	if (pOwner->pAttackComponent->endAttack) {
		pOwner->pAttackComponent->endAttack = false;
		pOwner->EndState();
		return;
	}
}

void AttackState::Exit(StateMachine* pOwner)
{
	//OutputDebugStringA("Exiting ATTACK State\n");
}