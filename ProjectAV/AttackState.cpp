#include "AttackState.h"
#include "StateMachine.h"
#include "Node.h" // Include Node to get other components


#include "Win.h" // For OutputDebugStringA

AttackState::AttackState(StateMachine* pOwner) : State()
{
}


void AttackState::Enter(StateMachine* pOwner)
{
	OutputDebugStringA("Entering ATTACK State\n");
	if (pOwner->attackComponents.size() > 0) {
		int randI = rand() % pOwner->attackComponents.size();
		pAttackComponent = pOwner->attackComponents[randI];
		time = 0.0f;
	}
}

void AttackState::Update(StateMachine* pOwner, float dt)
{
	time += dt;

	if (time > 3.0f) {
		pOwner->EndState();
		return;
	}
}

void AttackState::Exit(StateMachine* pOwner)
{
	OutputDebugStringA("Exiting ATTACK State\n");
}