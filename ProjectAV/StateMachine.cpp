#include "StateMachine.h"
#include "State.h"
#include "PhysicsCommon.h"
#include "Components.h"

#include "IdleState.h"
#include "FollowState.h"
#include "AttackState.h"
#include "StunState.h"
#include "StopState.h"

#include <stdexcept> 
#include <cassert>
#include <string>
#include "Win.h"
#include "PrefabManager.h"
#include "StaticSoundPlayer.h"



StateMachine::StateMachine(Node* owner, StateType initialState)
	: Component(owner) 
{
	RegisterState<IdleState>(StateType::IDLE);
	RegisterState<FollowState>(StateType::FOLLOW);
	RegisterState<AttackState>(StateType::ATTACK);
	RegisterState<StunState>(StateType::STUN);
	RegisterState<StopState>(StateType::STOP);

	ChangeState(initialState);
	if (currentStateType == StateType::NONE) {
		currentStateType = StateType::IDLE;
		ChangeState(StateType::IDLE); 
	}
	//Stop(3.0f);
	//for (const auto& component : pOwner->GetComponents())
	//{
	//	std::string tagName = "MOVEMENT";
	//	std::string tagName2 = "ATTACK";

	//	if (component->tag == tagName)
	//	{
	//		pMovementComponent = component.get();
	//	}
	//	else if (component->tag == tagName2)
	//	{
	//		attackComponents.push_back(component.get());
	//	}
	//}
	pPlayer = pOwner->GetRoot()->FindFirstChildByTag("PLAYER");
	basePos = pOwner->GetWorldPosition();
}
StateMachine::~StateMachine()
{

}
void StateMachine::Stun(float time)
{
	// dŸwiêk stuna 
	// zatrzymanie w miejscu
	stunTime = time;
	RequestStateChange(StateType::STUN);
}
void StateMachine::Stop(float time)
{
	stopTime = time;
	RequestStateChange(StateType::STOP);
}
void StateMachine::Update(float dt)
{
	if (timer < 2.0f && canDropPills)
	{
		timer += dt;
		//pOwner->SetWorldPosition(basePos);
		eatedPills = false;
		pOwner->GetComponent<Health>()->currentHealth = pOwner->GetComponent<Health>()->maxHealth;
	}

	if (hitted)
	{

	}

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
	if (eatedPills)
	{
		Die();
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
		velocity.SetX(velocity.GetX() * 0.97f);
		velocity.SetZ(velocity.GetZ() * 0.97f);
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
	/*pPlayer->GetComponent<PlayerController>()->abilitySlot3->GetComponent<Ability>()->killsCount++;
	pPlayer->GetComponent<Health>()->TakeDamage(-1);*/
	if (!isDead)
	{
		isDead = true;
		if(canDropPills)
		{
			Vector3 position = pOwner->GetLocalPosition();
			RRayCast ray = RRayCast(
				RVec3(pOwner->GetWorldPosition().x, pOwner->GetWorldPosition().y, pOwner->GetWorldPosition().z),
				Vec3(0.0f, -30.0f, 0.0f)
			);
			RayCastResult result;
			if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(ray, result,
				IgnoreMultipleBroadPhaseLayerFilter({ BroadPhaseLayers::ENEMY, BroadPhaseLayers::TRIGGER }),
				IgnoreMultipleObjectLayerFilter({ Layers::ENEMY, Layers::TRIGGER })))
			{
				Vec3 hitPos = ray.mOrigin + ray.mDirection * result.mFraction;
				Node* parent = pOwner->GetParent();
				DirectX::XMFLOAT3 hitPosFloat3(hitPos.GetX(), hitPos.GetY(), hitPos.GetZ());
				DirectX::XMMATRIX parentWorld = parent->GetWorldTransform();
				DirectX::XMMATRIX invParentWorld = DirectX::XMMatrixInverse(nullptr, parentWorld);

				DirectX::XMVECTOR worldVec = DirectX::XMLoadFloat3(&hitPosFloat3);
				DirectX::XMVECTOR localVec = DirectX::XMVector3Transform(worldVec, invParentWorld);

				DirectX::XMFLOAT3 localHitPos;
				DirectX::XMStoreFloat3(&localHitPos, localVec);

				float targetY = localHitPos.y + 4.0f;
				PrefabManager::InstantiateHealthCollectable(pOwner->GetParent(), Vector3(position.x + 0.8f, position.y, position.z + 0.8f), 0.3f, targetY);
				PrefabManager::InstantiateExpCollectable(pOwner->GetParent(), Vector3(position.x - 0.8f, position.y, position.z - 0.8f), 0.3f, targetY);
			}
			else
			{
				PrefabManager::InstantiateHealthCollectable(pOwner->GetParent(), Vector3(position.x + 0.8f, position.y, position.z + 0.8f), 0.3f, position.y);
				PrefabManager::InstantiateExpCollectable(pOwner->GetParent(), Vector3(position.x - 0.8f, position.y, position.z - 0.8f), 0.3f, position.y);
			}
		}
		pOwner->GetComponent<SoundEffectsPlayer>()->StopAll();
		StaticSoundPlayer::Get().Play("Sounds\\player\\damage1.wav", pOwner->GetWorldPosition(), 1.0f);
		pOwner->Destroy();
	}
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