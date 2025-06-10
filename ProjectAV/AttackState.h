#pragma once

#include "State.h" // Include the interface
#include "Component.h"

class AttackState : public State
{
public:
	// Constructor can take owner if needed, but often not necessary for simple states
	AttackState(StateMachine* pOwner);

	// Override virtual functions from IState
	virtual void Enter(StateMachine* pOwner) override;
	virtual void Update(StateMachine* pOwner, float dt) override;
	virtual void Exit(StateMachine* pOwner) override;
	virtual StateType GetType() const override { return StateType::IDLE; }

	float time;
};