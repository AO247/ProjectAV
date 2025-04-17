#include "IdleState.h"
#include "StateMachine.h"
#include "Node.h" // Include Node to get other components


#include <Windows.h> // For OutputDebugStringA

IdleState::IdleState(StateMachine* pOwner) : State()
{
}


void IdleState::Enter(StateMachine* pOwner)
{
	OutputDebugStringA("Entering IDLE State\n");
	time = 0.0f;

}

void IdleState::Update(StateMachine* pOwner, float dt)
{
	time += dt;

	if (time > 5.0f) {
		OutputDebugStringA("Switching to ATTACK State\n");
		pOwner->RequestStateChange(StateType::ATTACK);
	}
}

void IdleState::Exit(StateMachine* pOwner)
{
	OutputDebugStringA("Exiting IDLE State\n");
}