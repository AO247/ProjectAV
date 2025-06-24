#include "SpawnAttack.h"
#include "Node.h"       
#include "PrefabManager.h"
#include "EnemyConfig.h"
#include "AnimationComponent.h"

namespace dx = DirectX;
SpawnAttack::SpawnAttack(Node* owner, Node* player, std::string tag)
	: Component(owner, std::move(tag)), player(player)
{
}

void SpawnAttack::Attack(float dt)
{
	AnimationComponent* animComp = pOwner->GetComponent<AnimationComponent>();

	Vector3 playerPos = player->GetWorldPosition();
	if (undercover)
	{
		Vector3 playerPos = player->GetWorldPosition();

		sm::Vector3 facingDirection = sm::Vector3(playerPos)
			- sm::Vector3(pOwner->GetWorldPosition());
		facingDirection.Normalize();

		float targetYaw = atan2f(facingDirection.x, facingDirection.z);

		Quat q = Quat::sEulerAngles(Vec3(0.0f, targetYaw, 0.0f));
		PhysicsCommon::physicsSystem->GetBodyInterface().SetRotation(pOwner->GetComponent<Rigidbody>()->GetBodyID(), q, EActivation::Activate);
	}
	timer += dt;
	if (timer >= cooldownTime)
	{

		int randomAttack = std::rand() % 4;
		if (randomAttack == 0) {
			animComp->PlayAnimation(EnemyAnimationIndices::MAGE_ATTACK1, 0.5f, false);
		}
		else if (randomAttack == 1) {
			animComp->PlayAnimation(EnemyAnimationIndices::MAGE_ATTACK2, 0.5f, false);
		}
		else if (randomAttack == 2) {
			animComp->PlayAnimation(EnemyAnimationIndices::MAGE_ATTACK3, 0.5f, false);
		}
		else {
			animComp->PlayAnimation(EnemyAnimationIndices::MAGE_ATTACK4, 0.5f, false);
		}



		timer = 0.0f;
		endAttack = false;
		Vector3 pos = pOwner->GetLocalPosition();
		pos += pOwner->Forward() * 5.0f;
		Node* enemy = PrefabManager::InstantiateExplosiveEnemy(pOwner->GetParent(), Vector3(pos.x, pos.y, pos.z));
		enemy->MoveToTop();
		soundTimer = 0.6f;
		soundPlayed = false;
	}
	if (soundTimer > 0.0f)
	{
		soundTimer -= dt;
	}
	else if (!soundPlayed)
	{
		if (pOwner->GetComponent<SoundEffectsPlayer>())
		{
			float randSound = (rand() % 4 + 4);
			pOwner->GetComponent<SoundEffectsPlayer>()->Play(randSound);
		}
		soundPlayed = true;
	}
}



void SpawnAttack::DrawImGuiControls()
{
	ImGui::Text("Tag: %s", tag.c_str());
	ImGui::InputFloat("Attack Timer", &timer);
	ImGui::Checkbox("Attacked", &attacked);
}