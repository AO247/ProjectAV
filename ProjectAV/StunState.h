#pragma once

#include "State.h"
#include "Component.h"

class StunState : public State
{
public:
	StunState(StateMachine* pOwner);

	virtual void Enter(StateMachine* pOwner) override;
	virtual void Update(StateMachine* pOwner, float dt) override;
	virtual void Exit(StateMachine* pOwner) override;
	virtual StateType GetType() const override { return StateType::STUN; }

	float time;
};