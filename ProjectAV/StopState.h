#pragma once

#include "State.h"
#include "Component.h"

class StopState : public State
{
public:
	StopState(StateMachine* pOwner);

	virtual void Enter(StateMachine* pOwner) override;
	virtual void Update(StateMachine* pOwner, float dt) override;
	virtual void Exit(StateMachine* pOwner) override;
	virtual StateType GetType() const override { return StateType::STOP; }

	float time;
};