#pragma once

#include "State.h"

class FollowState : public State
{
public:
	FollowState(StateMachine* pOwner);

	virtual void Enter(StateMachine* pOwner) override;
	virtual void Update(StateMachine* pOwner, float dt) override;
	virtual void Exit(StateMachine* pOwner) override;
	virtual StateType GetType() const override { return StateType::IDLE; }
	float time;
};