#pragma once

#include "Component.h" // Include your base Component class
#include "StateType.h"      // Include the enum definition
#include <memory>           // For unique_ptr
#include <unordered_map>    // For state factory
#include <functional>       // For std::function
#include "Node.h"

// Forward declaration
class State;

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

	// --- Accessors needed by States ---
	Node* GetOwnerNode() const; // Convenience accessor
	//std::vector<std::unique_ptr<Component>> movementComponents;
	//std::vector<std::unique_ptr<Component>> attackComponents;
	Node* pPlayer = nullptr; // Node representing the player capsule/origin

	float followDistance = 40.0f;
	float attackRange = 8.0f; // Distance to trigger attack state
	Component* pMovementComponent = nullptr; // Pointer to the movement component
	std::vector<Component*> movementComponents;
	std::vector<Component*> attackComponents;
	template<typename T> T* GetComponent() const; // Keep declaration
	Component* AddComponent(std::unique_ptr<Component> pComponent); // Keep declaration
	const std::vector<std::unique_ptr<Component>>& GetComponents() const; // Keep declaration
	void UpdateComponents();
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
	
	std::vector<std::unique_ptr<Component>> components;


	virtual void DrawImGuiControls() override;

	// Helper to register states in the factory during construction
	template<typename StateClass> void RegisterState(StateType type) {
		static_assert(std::is_base_of<State, StateClass>::value, "StateClass must derive from State");
		stateFactory[type] = [](StateMachine* owner) {
			return std::make_unique<StateClass>(owner); // Pass owner to state constructor if needed
			};
	}
};
template<typename T> T* StateMachine::GetComponent() const
{
    static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
    for (const auto& comp : components)
    {
        if (auto* p = dynamic_cast<T*>(comp.get()))
        {
            return p;
        }
    }
    return nullptr;
}