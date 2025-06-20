#include "SpawnAttack.h"
#include "Node.h"       
#include "PrefabManager.h"

namespace dx = DirectX;
SpawnAttack::SpawnAttack(Node* owner, Node* player, std::string tag)
	: Component(owner, std::move(tag)), player(player)
{
}

void SpawnAttack::Attack(float dt)
{
	Vector3 playerPos = player->GetWorldPosition();

	sm::Vector3 facingDirection = sm::Vector3(playerPos)
		- sm::Vector3(pOwner->GetWorldPosition());
	facingDirection.Normalize();

	float targetYaw = atan2f(facingDirection.x, facingDirection.z);

	Quat q = Quat::sEulerAngles(Vec3(0.0f, targetYaw, 0.0f));
	PhysicsCommon::physicsSystem->GetBodyInterface().SetRotation(pOwner->GetComponent<Rigidbody>()->GetBodyID(), q, EActivation::Activate);

	timer += dt;
	if (timer >= cooldownTime)
	{
		timer = 0.0f;
		endAttack = false;
		Vector3 pos = pOwner->GetLocalPosition();
		pos += pOwner->Forward() * 5.0f;
		Node* enemy = PrefabManager::InstantiateExplosiveEnemy(pOwner->GetParent(), Vector3(pos.x, pos.y, pos.z), 1.0f);
		enemy->MoveToTop();
	}

}



void SpawnAttack::DrawImGuiControls()
{
	ImGui::Text("Tag: %s", tag.c_str());
	ImGui::InputFloat("Attack Timer", &timer);
	ImGui::Checkbox("Attacked", &attacked);
}