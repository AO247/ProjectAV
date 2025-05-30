#pragma once

#include "Component.h" // Include your base Component class
#include "StateType.h"      // Include the enum definition
#include <memory>           // For unique_ptr
#include <unordered_map>    // For state factory
#include <functional>       // For std::function
#include "Node.h"
#include <SimpleMath.h>

// Forward declaration
class State;
namespace sm = DirectX::SimpleMath;

class StateMachine : public Component
{
public:
	StateMachine(Node* owner, StateType initialState = StateType::IDLE);
	virtual ~StateMachine() override; // Override if cleanup needed

	// --- Core Methods ---
	virtual void Update(float dt) override; // Override from Component

	// --- State Management ---
	void RequestStateChange(StateType nextState);
	StateType GetCurrentStateType() const;

	sm::Vector3 pos;
	sm::Vector3 cen;
	// --- Accessors needed by States ---
	Node* GetOwnerNode() const; // Convenience accessor
	//std::vector<std::unique_ptr<Component>> movementComponents;
	//std::vector<std::unique_ptr<Component>> attackComponents;
	Node* pPlayer = nullptr; // Node representing the player capsule/origin

	float followDistance = 40.0f;
	float attackRange = 8.0f; // Distance to trigger attack state
	Component* pMovementComponent = nullptr; // Pointer to the movement component
	Component* pAttackComponent = nullptr;
	std::vector<Component*> attackComponents;
	void EndState(); // Call to end the current state and reset to idle
private:
	// Performs the actual state transition
	void ChangeState(StateType nextState);

	// --- State Data ---
	std::unique_ptr<State> currentState = nullptr;
	std::unique_ptr<State> previousState = nullptr; // For pending state changes

	StateType previousStateType = StateType::NONE; // Previous state type for debugging
	StateType currentStateType = StateType::NONE;
	StateType requestedNextStateType = StateType::NONE; // State requested for next frame


	// --- State Factory ---
	// Maps a StateType enum to a function that creates the corresponding IState object
	using StateFactoryFunc = std::function<std::unique_ptr<State>(StateMachine*)>;

	std::unordered_map<StateType, StateFactoryFunc> stateFactory;


	virtual void DrawImGuiControls() override;

	// Helper to register states in the factory during construction
	template<typename StateClass> void RegisterState(StateType type) {
		static_assert(std::is_base_of<State, StateClass>::value, "StateClass must derive from State");
		stateFactory[type] = [](StateMachine* owner) {
			return std::make_unique<StateClass>(owner); // Pass owner to state constructor if needed
			};
	}
};
