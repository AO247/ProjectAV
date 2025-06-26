#include "FourFireBallAttack.h"
#include "Node.h"       
#include "PrefabManager.h"

namespace dx = DirectX;
FourFireBallAttack::FourFireBallAttack(Node* owner, Node* player, std::string tag)
	: Component(owner, std::move(tag)), player(player)
{
}

void FourFireBallAttack::Attack(float dt)
{
	if (player == NULL && player == nullptr) return;
	Vector3 playerPos = player->GetWorldPosition();

	sm::Vector3 facingDirection = sm::Vector3(playerPos)
		- sm::Vector3(pOwner->GetWorldPosition());
	facingDirection.Normalize();

	float targetYaw = atan2f(facingDirection.x, facingDirection.z);

	Quat q = Quat::sEulerAngles(Vec3(0.0f, targetYaw, 0.0f));
	PhysicsCommon::physicsSystem->GetBodyInterface().SetRotation(pOwner->GetComponent<Rigidbody>()->GetBodyID(), q, EActivation::Activate);
	shootSoundTimer -= dt;

	if (timer == 0)
	{
		if (pOwner->GetComponent<SoundEffectsPlayer>()) {
			if (shootSoundTimer <= 0.0f)
			{
				float randSound = (rand() % 2 + 4);
				pOwner->GetComponent<SoundEffectsPlayer>()->Play(randSound);
				shootSoundTimer = shootSoundInterval;
			}
		}
		pOwner->GetComponent<AnimationComponent>()->PlayAnimation(2, false);
	}
	timer += dt;
	if (timer >= wholeAttackTime) {
		attacked = false;
		timer = 0.0f;
		endAttack = true;
		return;
	}

	if (timer > spawnTime1)
	{
		Vector3 pos = pOwner->GetWorldPosition();
		pos += pOwner->Forward() * 5.0f;
		fireBall1 = PrefabManager::InstantiateFireBall(Vector3(pos.x, pos.y, pos.z), 0.2f);
	}
	if (timer > spawnTime2)
	{
		Vector3 pos = pOwner->GetWorldPosition();
		pos += pOwner->Forward() * 5.0f;
		fireBall2 = PrefabManager::InstantiateFireBall(Vector3(pos.x, pos.y, pos.z), 0.2f);
	}
	if (timer > spawnTime3)
	{
		Vector3 pos = pOwner->GetWorldPosition();
		pos += pOwner->Forward() * 5.0f;
		fireBall3 = PrefabManager::InstantiateFireBall(Vector3(pos.x, pos.y, pos.z), 0.2f);
	}
	if (timer > spawnTime4)
	{
		Vector3 pos = pOwner->GetWorldPosition();
		pos += pOwner->Forward() * 5.0f;
		fireBall4 = PrefabManager::InstantiateFireBall(Vector3(pos.x, pos.y, pos.z), 0.2f);
	}

	if (timer > shootTime1)
	{
		fireBall1->GetComponent<FireBall>()->ignore = pOwner;
		playerPos.y += 2.0f;
		Vector3 dir = playerPos - pOwner->GetWorldPosition();
		dir.Normalize();
		dir *= bulletSpeed;
		PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(fireBall1->GetComponent<Rigidbody>()->GetBodyID(), Vec3(dir.x, dir.y, dir.z));
	}
	if (timer > shootTime2)
	{
		fireBall2->GetComponent<FireBall>()->ignore = pOwner;
		playerPos.y += 2.0f;
		Vector3 dir = playerPos - pOwner->GetWorldPosition();
		dir.Normalize();
		dir *= bulletSpeed;
		PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(fireBall2->GetComponent<Rigidbody>()->GetBodyID(), Vec3(dir.x, dir.y, dir.z));
	}
	if (timer > shootTime3)
	{
		fireBall3->GetComponent<FireBall>()->ignore = pOwner;
		playerPos.y += 2.0f;
		Vector3 dir = playerPos - pOwner->GetWorldPosition();
		dir.Normalize();
		dir *= bulletSpeed;
		PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(fireBall3->GetComponent<Rigidbody>()->GetBodyID(), Vec3(dir.x, dir.y, dir.z));
	}
	if (timer > shootTime4)
	{
		fireBall4->GetComponent<FireBall>()->ignore = pOwner;
		playerPos.y += 2.0f;
		Vector3 dir = playerPos - pOwner->GetWorldPosition();
		dir.Normalize();
		dir *= bulletSpeed;
		PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(fireBall4->GetComponent<Rigidbody>()->GetBodyID(), Vec3(dir.x, dir.y, dir.z));
	}
}



void FourFireBallAttack::DrawImGuiControls()
{
	ImGui::Text("Tag: %s", tag.c_str());
	ImGui::InputFloat("Damage", &damage);
	ImGui::InputFloat("Whole Attack Time", &wholeAttackTime);
	ImGui::InputFloat("Knockback Force", &knockbackForce);
	ImGui::InputFloat("Attack Timer", &timer);
	ImGui::Checkbox("Attacked", &attacked);
}