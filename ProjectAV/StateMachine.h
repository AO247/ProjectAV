#pragma once

#include "Component.h" 
#include "StateType.h"      
#include <memory>           
#include <unordered_map>    
#include <functional>       
#include "Node.h"
#include <SimpleMath.h>

class State;
namespace sm = DirectX::SimpleMath;

class StateMachine : public Component
{
public:
	StateMachine(Node* owner, StateType initialState = StateType::IDLE);
	virtual ~StateMachine() override;

	virtual void Update(float dt) override; 

	void RequestStateChange(StateType nextState);
	StateType GetCurrentStateType() const;

	sm::Vector3 pos;
	sm::Vector3 cen;
	Node* GetOwnerNode() const;
	Node* pPlayer = nullptr;

	float followDistance = 40.0f;
	float attackRange = 8.0f;
	float stunTime = 1.0f;
	float stopTime = 1.0f;

	bool isFlying = false;
	bool isDead = false;

	Component* pMovementComponent = nullptr;
	Component* pAttackComponent = nullptr;
	std::vector<Component*> attackComponents;
	void EndState();
	void Die();
	void ChangeState(StateType nextState);
	void Stun(float time);
	void Stop(float time);
	float timer = 0.0f;

private:


	std::unique_ptr<State> currentState = nullptr;
	std::unique_ptr<State> previousState = nullptr; 

	StateType previousStateType = StateType::NONE; 
	StateType currentStateType = StateType::NONE;
	StateType requestedNextStateType = StateType::NONE; 

	using StateFactoryFunc = std::function<std::unique_ptr<State>(StateMachine*)>;

	std::unordered_map<StateType, StateFactoryFunc> stateFactory;


	virtual void DrawImGuiControls() override;

	template<typename StateClass> void RegisterState(StateType type) {
		static_assert(std::is_base_of<State, StateClass>::value, "StateClass must derive from State");
		stateFactory[type] = [](StateMachine* owner) {
			return std::make_unique<StateClass>(owner);
			};
	}
};
