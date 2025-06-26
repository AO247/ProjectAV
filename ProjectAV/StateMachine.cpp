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
	//basePos = pOwner->GetWorldPosition();
}
StateMachine::~StateMachine()
{

}
void StateMachine::Stun(float time)
{
	StateMachine* statemachine = pOwner->GetComponent <StateMachine>();

	// dŸwiêk stuna 
	// zatrzymanie w miejscu


	if (statemachine->enemyType == EnemyType::BASIC) {
		pOwner->GetComponent<AnimationComponent>()->PlayAnimation(2);		//stun basic

	}/*
	if (statemachine->enemyType == EnemyType::RANGED) {
		pOwner->GetComponent<AnimationComponent>()->PlayAnimation(2); 
	}*/
	if (statemachine->enemyType == EnemyType::TANK) {
		pOwner->GetComponent<AnimationComponent>()->PlayAnimation(2);	//stun tank
	}
	/*if (statemachine->enemyType == EnemyType::EXPLOSIVE) {
		pOwner->GetComponent<AnimationComponent>()->PlayAnimation(8); 
	}*/
	if (statemachine->enemyType == EnemyType::FRENZY) {
		pOwner->GetComponent<AnimationComponent>()->PlayAnimation(10);		//stun fast
	}
	/*if (statemachine->enemyType == EnemyType::FLYING) {
		pOwner->GetComponent<AnimationComponent>()->PlayAnimation(4);
	}*/
	if (statemachine->enemyType == EnemyType::MAGE) {
		pOwner->GetComponent<AnimationComponent>()->PlayAnimation(6);		//stun mage
	}

	stunTime = time;
	RequestStateChange(StateType::STUN);


}

void StateMachine::Stop(float time)
{
	StateMachine* statemachine = pOwner->GetComponent <StateMachine>();

	if (statemachine->enemyType == EnemyType::FRENZY) {
		pOwner->GetComponent<AnimationComponent>()->PlayAnimation(10);	//stop fast
	}
	if (statemachine->enemyType == EnemyType::TANK) {
		pOwner->GetComponent<AnimationComponent>()->PlayAnimation(2);	//stop tank
	}
	if (statemachine->enemyType == EnemyType::BASIC) {
		pOwner->GetComponent<AnimationComponent>()->PlayAnimation(2);	//stop basic

	}
	if (statemachine->enemyType == EnemyType::MAGE) {
		pOwner->GetComponent<AnimationComponent>()->PlayAnimation(6);	//stop mage
	}
	stopTime = time;
	RequestStateChange(StateType::STOP);
}

void StateMachine::Update(float dt)
{
	if (attackComponents.size() > 1)
	{
		if (attackCooldownTimer <= 0.0f)
		{
			attackCooldownTimer = 4.0f;
			int randIndex = rand() % attackComponents.size();
			pAttackComponent = attackComponents[randIndex];
			attackRange = pAttackComponent->attackRange;
		}
		attackCooldownTimer -= dt;
	}
	else
	{
		pAttackComponent = attackComponents[0];
	}

	if (timer < 2.0f && canDropPills)
	{
		timer += dt;
		/*PhysicsCommon::physicsSystem->GetBodyInterface().SetPosition(pOwner->GetComponent<Rigidbody>()->GetBodyID(), 
			Vec3(basePos.x, basePos.y, basePos.z), EActivation::Activate);*/
		PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(pOwner->GetComponent<Rigidbody>()->GetBodyID(),
			Vec3(0.0f, 0.0f, 0.0f));
		eatedPills = false;
		pOwner->GetComponent<Health>()->currentHealth = pOwner->GetComponent<Health>()->maxHealth;
	}
	if (attackCooldownTimer > 0.0f)
	{
		attackCooldownTimer -= dt;
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
		float dampingFactor = powf(flyingDamping, dt);
		velocity *= dampingFactor;
		PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(pOwner->GetComponent<Rigidbody>()->GetBodyID(), velocity);
		Vector3 pos = pOwner->GetWorldPosition();

		RRayCast ray = RRayCast(
			RVec3(pos.x, pos.y, pos.z),
			RVec3(0.0f, -100, 0.0f)
		);
		RayCastResult result;
		if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(ray, result, SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::GROUND), SpecifiedObjectLayerFilter(Layers::GROUND)))
		{
			Vec3 position = ray.mOrigin + ray.mDirection * result.mFraction;
			pos.y = position.GetY() + pMovementComponent->flyingHeight;
			Vector3 force = pos - pOwner->GetWorldPosition();
			if (force.Length() < 3.0f)
			{
				pMovementComponent->canAttack = true;
			}
			else
			{
				pMovementComponent->canAttack = false;
			}
			OutputDebugStringA(("force length: " + std::to_string(force.Length())).c_str());
		}
		else
		{
			Vector3 position = pPlayer->GetWorldPosition();
			pos.y = position.y + pMovementComponent->flyingHeight - 2.0f;
			Vector3 force = pos - pOwner->GetWorldPosition();
			if (force.Length() < 3.0f)
			{
				pMovementComponent->canAttack = true;
			}
			else
			{
				pMovementComponent->canAttack = false;
			}
		}

	}
	else
	{
		if(pMovementComponent->GroundCheck())
		{
			Vec3 velocity = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(pOwner->GetComponent<Rigidbody>()->GetBodyID());
			float dampingFactor = powf(groundDamping, dt);
			velocity.SetX(velocity.GetX() * dampingFactor);
			velocity.SetZ(velocity.GetZ() * dampingFactor);
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
				PrefabManager::InstantiateHealthCollectable(pOwner->GetParent(), Vector3(position.x + 0.8f, position.y, position.z + 0.8f), 0.7f, targetY);
				PrefabManager::InstantiateExpCollectable(pOwner->GetParent(), Vector3(position.x - 0.8f, position.y, position.z - 0.8f), 0.7f, targetY);
			}
			else
			{
				PrefabManager::InstantiateHealthCollectable(pOwner->GetParent(), Vector3(position.x + 0.8f, position.y, position.z + 0.8f), 0.7f, position.y);
				PrefabManager::InstantiateExpCollectable(pOwner->GetParent(), Vector3(position.x - 0.8f, position.y, position.z - 0.8f), 0.7f, position.y);
			}
		}
		pOwner->GetComponent<SoundEffectsPlayer>()->StopAll();
		int randSound = rand() & 4;
		switch (randSound) {
		case 0:
			StaticSoundPlayer::Get().Play("Sounds\\enemies\\death1.wav", pOwner->GetWorldPosition(), 0.8f);
			break;
		case 1:
			StaticSoundPlayer::Get().Play("Sounds\\enemies\\death2.wav", pOwner->GetWorldPosition(), 0.8f);
			break;
		case 2:
			StaticSoundPlayer::Get().Play("Sounds\\enemies\\death3.wav", pOwner->GetWorldPosition(), 0.8f);
			break;
		case 3:
			StaticSoundPlayer::Get().Play("Sounds\\enemies\\death4.wav", pOwner->GetWorldPosition(), 0.8f);
			break;
		}
		
		PrefabManager::InstantiateEnemyKillParticles(pOwner->GetParent(), Vector3(pOwner->GetWorldPosition().x, pOwner->GetWorldPosition().y, pOwner->GetWorldPosition().z), 1.0f);

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
	ImGui::SliderFloat("Flying Damping", &flyingDamping, 0.0f, 0.99f);
	ImGui::SliderFloat("Ground Damping", &groundDamping, 0.0f, 0.99f);
}