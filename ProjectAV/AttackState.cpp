#include "AttackState.h"
#include "StateMachine.h"
#include "Node.h" // Include Node to get other components


#include <Windows.h> // For OutputDebugStringA

AttackState::AttackState(StateMachine* pOwner) : State()
{
}


void AttackState::Enter(StateMachine* pOwner)
{
	OutputDebugStringA("Entering IDLE State\n");
	time = 0.0f;

}

void AttackState::Update(StateMachine* pOwner, float dt)
{
	time += dt;

	if (time > 5.0f) {
		OutputDebugStringA("Switching to FOLLOW State\n");
		pOwner->RequestStateChange(StateType::FOLLOW);
	}
}

void AttackState::Exit(StateMachine* pOwner)
{
	OutputDebugStringA("Exiting IDLE State\n");
}