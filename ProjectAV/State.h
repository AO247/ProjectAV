#pragma once

#include "StateType.h" 
#include <memory>      

class StateMachine;
class Node;


class State
{
public:
	virtual ~State() = default;

	virtual void Enter(StateMachine* pOwner) = 0;

	virtual void Update(StateMachine* pOwnert, float dt) = 0;

	virtual void Exit(StateMachine* pOwner) = 0;

	virtual StateType GetType() const = 0;
};