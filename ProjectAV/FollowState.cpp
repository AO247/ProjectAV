#include "FollowState.h"
#include "StateMachine.h"
#include "Node.h" // Include Node to get other components


#include <Windows.h> // For OutputDebugStringA

FollowState::FollowState(StateMachine* pOwner) : State()
{
}


void FollowState::Enter(StateMachine* pOwner)
{
	OutputDebugStringA("Entering IDLE State\n");
	time = 0.0f;

}

void FollowState::Update(StateMachine* pOwner, float dt)
{
	time += dt;

	if (time > 5.0f) {
		OutputDebugStringA("Switching to IDLE State\n");
		pOwner->RequestStateChange(StateType::IDLE);
	}
}

void FollowState::Exit(StateMachine* pOwner)
{
	OutputDebugStringA("Exiting IDLE State\n");
}