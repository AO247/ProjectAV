#include "StateMachine.h"
#include "State.h"

#include "IdleState.h"
#include "FollowState.h"
#include "AttackState.h"

#include <stdexcept> // For runtime_error
#include <cassert>
#include <string>
#include <Windows.h> // For OutputDebugStringA



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
	
	pPlayerNode = owner->GetRoot()->FindFirstChildByTag("Player");
}
StateMachine::~StateMachine()
{

}
void StateMachine::UpdateComponents()
{
	for (const auto& component : components)
	{
		component->tag = "Movement";
		std::string tagName = "Movement";
		std::string tagName2 = "Attack";

		if (component->tag == tagName)
		{
			pMovementComponent = component.get(); // Store the movement component
			// Store a raw pointer to the component
			movementComponents.push_back(component.get());
		}
		else if (component->tag == tagName2)
		{
			// Store a raw pointer to the component
			attackComponents.push_back(component.get());
		}
	}
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

Component* StateMachine::AddComponent(std::unique_ptr<Component> pComponent) {
    assert(pComponent);
    components.push_back(std::move(pComponent));
    return components.back().get();
}
const std::vector<std::unique_ptr<Component>>& StateMachine::GetComponents() const { return components; }

void StateMachine::DrawImGuiControls()
{
	ImGui::Text("State Machine Properties:");
	ImGui::InputFloat("Start Following distance", &followDistance);
	ImGui::InputFloat("Attack Range", &attackRange);

	if (ImGui::CollapsingHeader("Components", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (components.empty()) {
			ImGui::TextDisabled("No components attached.");
		}
		else {
			int compIndex = 0;
			for (const auto& comp : components) {
				if (comp) {
					// Create a unique ID for the component header
					std::string compLabel = typeid(*comp).name();
					// Remove "class " prefix if present (platform dependent)
					if (compLabel.rfind("class ", 0) == 0) {
						compLabel = compLabel.substr(6);
					}
					compLabel += "##" + std::to_string(compIndex++); // Add unique ID

					// Make each component collapsible
					if (ImGui::TreeNode(compLabel.c_str()))
					{
						// --- Call the component's ImGui draw function ---
						comp->DrawImGuiControls();
						// --- End Call ---

						ImGui::TreePop();
					}
				}
			}
		}
	}
	//ImGui::Checkbox("Jumped", &jumped); // Display jump status
}