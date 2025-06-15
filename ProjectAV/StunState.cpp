#include "StunState.h"
#include "StateMachine.h"
#include "Node.h" 


#include "Win.h" 

StunState::StunState(StateMachine* pOwner) : State()
{
}


void StunState::Enter(StateMachine* pOwner)
{
	time = 0.0f;
}

void StunState::Update(StateMachine* pOwner, float dt)
{
	time += dt;

	if (time > 2.0f)
	{
		pOwner->EndState();
	}
}

void StunState::Exit(StateMachine* pOwner)
{
}