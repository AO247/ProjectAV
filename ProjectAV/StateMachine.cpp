#include "StateMachine.h"
#include "State.h"
#include "PhysicsCommon.h"
#include "Components.h"

#include "IdleState.h"
#include "FollowState.h"
#include "AttackState.h"
#include "StunState.h"

#include <stdexcept> 
#include <cassert>
#include <string>
#include "Win.h"



StateMachine::StateMachine(Node* owner, StateType initialState)
	: Component(owner) 
{
	RegisterState<IdleState>(StateType::IDLE);
	RegisterState<FollowState>(StateType::FOLLOW);
	RegisterState<AttackState>(StateType::ATTACK);
	RegisterState<StunState>(StateType::STUN);

	ChangeState(initialState);
	if (currentStateType == StateType::NONE) {
		currentStateType = StateType::IDLE;
		ChangeState(StateType::IDLE); 
	}
	for (const auto& component : pOwner->GetComponents())
	{
		std::string tagName = "MOVEMENT";
		std::string tagName2 = "ATTACK";

		if (component->tag == tagName)
		{
			pMovementComponent = component.get();
		}
		else if (component->tag == tagName2)
		{
			attackComponents.push_back(component.get());
		}
	}
	pPlayer = pOwner->GetRoot()->FindFirstChildByTag("PLAYER");
}
StateMachine::~StateMachine()
{

}


void StateMachine::Update(float dt)
{


	if (currentState)
	{
		currentState->Update(this, dt);
	}
	else
	{
		if (previousState) {
			ChangeState(previousStateType);
		}
		else 
		{
			OutputDebugStringA("StateMachine Warning: currentState is null in Update!\n");
			RequestStateChange(StateType::IDLE);
		}
		
	}
	if (pOwner->GetLocalPosition().y < -50.0f)
	{
		pOwner->Destroy();
	}
	if (isFlying) {
		Vec3 velocity = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(pOwner->GetComponent<Rigidbody>()->GetBodyID());
		velocity *= 0.95f;
		PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(pOwner->GetComponent<Rigidbody>()->GetBodyID(),velocity);
	}
	else
	{
		if(pMovementComponent->GroundCheck())
		{
		Vec3 velocity = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(pOwner->GetComponent<Rigidbody>()->GetBodyID());
		velocity *= 0.97f;
		PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(pOwner->GetComponent<Rigidbody>()->GetBodyID(), velocity);
		}
	}
}

void StateMachine::RequestStateChange(StateType nextState)
{

	if (nextState != currentStateType) 
	{
		ChangeState(nextState); 
	}
}

void StateMachine::EndState()
{

	if (currentState)
	{
		if (previousState) {
			ChangeState(previousStateType);
		}
		else
		{

			OutputDebugStringA("StateMachine Warning: currentState is null in Update!\n");

			RequestStateChange(StateType::IDLE);
		}
	}
}

void StateMachine::ChangeState(StateType nextStateType)
{

	auto it = stateFactory.find(nextStateType);
	if (it == stateFactory.end())
	{
		OutputDebugStringA(("StateMachine Error: Attempted to change to unregistered state type: " + std::to_string((int)nextStateType) + "\n").c_str());
		return;
	}

	if (currentState)
	{
		currentState->Exit(this);
	}

	previousState = std::move(currentState);
	previousStateType = currentStateType; 
	currentState = it->second(this); 
	currentStateType = nextStateType; 


	if (currentState)
	{
		currentState->Enter(this);
	}
	else
	{
		
		OutputDebugStringA(("StateMachine Error: Factory failed to create state type: " + std::to_string((int)nextStateType) + "\n").c_str());
		currentStateType = StateType::NONE;
	}
}

StateType StateMachine::GetCurrentStateType() const
{
	return currentStateType;
}

void StateMachine::Die()
{
	pPlayer->GetComponent<PlayerController>()->abilitySlot3->GetComponent<Ability>()->killsCount++;
	pPlayer->GetComponent<Health>()->TakeDamage(-1);
	pOwner->Destroy();
}

Node* StateMachine::GetOwnerNode() const
{
	return GetOwner();
}

void StateMachine::DrawImGuiControls()
{
	ImGui::Text("State Machine Properties:");
	ImGui::InputFloat("Start Following distance", &followDistance);
	ImGui::InputFloat("Attack Range", &attackRange);
}