#include "StopState.h"
#include "StateMachine.h"
#include "Node.h" 


#include "Win.h" 

StopState::StopState(StateMachine* pOwner) : State()
{
}


void StopState::Enter(StateMachine* pOwner)
{
	time = 0.0f;
}

void StopState::Update(StateMachine* pOwner, float dt)
{
	time += dt;

	if (time > 0.5f)
	{
		pOwner->RequestStateChange(StateType::IDLE);
	}
}

void StopState::Exit(StateMachine* pOwner)
{
}