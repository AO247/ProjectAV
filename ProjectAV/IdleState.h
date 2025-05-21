#pragma once

#include "State.h" // Include the interface
#include <SimpleMath.h>
namespace sm = DirectX::SimpleMath;

class IdleState : public State
{
public:
	// Constructor can take owner if needed, but often not necessary for simple states
	IdleState(StateMachine* pOwner);

	// Override virtual functions from IState
	virtual void Enter(StateMachine* pOwner) override;
	virtual void Update(StateMachine* pOwner, float dt) override;
	virtual void Exit(StateMachine* pOwner) override;
	virtual StateType GetType() const override { return StateType::IDLE; }
	sm::Vector3 previousPos = { 0.0f, 0.0f, 0.0f };
	float wanderRradius = 1.5f;
	float wanderCenterDistance = 6.0f;
	float wanderAngleChange = 25.0f;
	float wanderAngle;
	float time;
};