#include "StateMachine.h"
#include "State.h"
#include "PhysicsCommon.h"

#include "IdleState.h"
#include "FollowState.h"
#include "AttackState.h"

#include <stdexcept> // For runtime_error
#include <cassert>
#include <string>
#include "Win.h"// For OutputDebugStringA



StateMachine::StateMachine(Node* owner, StateType initialState)
	: Component(owner) // Request initial state for first frame
{
	RegisterState<IdleState>(StateType::IDLE);
	RegisterState<FollowState>(StateType::FOLLOW);
	RegisterState<AttackState>(StateType::ATTACK);
	ChangeState(initialState);
	if (currentStateType == StateType::NONE) {
		currentStateType = StateType::IDLE; // Default fallback
		ChangeState(StateType::IDLE); // Try changing to IDLE if initial faile
	}
	//std::vector<std::unique_ptr<Component>> components = pOwner->GetComponents();
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

	// 2. Update the Current State
	if (currentState) // Ensure state exists
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
			// This shouldn't happen after the constructor logic, but safety check
			OutputDebugStringA("StateMachine Warning: currentState is null in Update!\n");
			// Maybe try to force back to IDLE?
			RequestStateChange(StateType::IDLE);
		}
		
	}
	if (pOwner->GetLocalPosition().y < -50.0f)
	{
		pOwner->Destroy();
	}
	if (isFlying) {
		Vec3 velocity = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(pOwner->GetComponent<Rigidbody>()->GetBodyID());
		velocity *= 0.95f; // Dampen velocity by 5% each frame
		PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(pOwner->GetComponent<Rigidbody>()->GetBodyID(),velocity);
	}
}

void StateMachine::RequestStateChange(StateType nextState)
{
	// Store the request. It will be processed at the start of the next Update.
	// This prevents changing state multiple times within a single frame's update loop.
	if (nextState != currentStateType) // Don't request change to the same state
	{
		ChangeState(nextState); // Optional: immediate change if desired
	}
}

void StateMachine::EndState()
{
	// End the current state and reset to idle
	if (currentState)
	{
		if (previousState) {
			ChangeState(previousStateType);
		}
		else
		{
			// This shouldn't happen after the constructor logic, but safety check
			OutputDebugStringA("StateMachine Warning: currentState is null in Update!\n");
			// Maybe try to force back to IDLE?
			RequestStateChange(StateType::IDLE);
		}
	}
}

void StateMachine::ChangeState(StateType nextStateType)
{
	// Find the factory function for the requested state
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
	previousStateType = currentStateType; // Store the previous state type
	currentState = it->second(this); // Calls the lambda stored in the map
	currentStateType = nextStateType; // Update the current type enum

	// Enter the new state
	if (currentState)
	{
		currentState->Enter(this);
	}
	else
	{
		// Factory failed to create state? Should not happen if registration is correct.
		OutputDebugStringA(("StateMachine Error: Factory failed to create state type: " + std::to_string((int)nextStateType) + "\n").c_str());
		currentStateType = StateType::NONE; // Mark as invalid state
	}
}

StateType StateMachine::GetCurrentStateType() const
{
	return currentStateType;
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