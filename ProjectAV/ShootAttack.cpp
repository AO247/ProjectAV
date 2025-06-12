#include "ShootAttack.h"
#include "Node.h"       
#include "PrefabManager.h"

namespace dx = DirectX;
ShootAttack::ShootAttack(Node* owner, std::string tag)
	: Component(owner, std::move(tag))
{
	player = pOwner->GetRoot()->FindFirstChildByTag("PLAYER");
	attackRange = 45.0f;
}

void ShootAttack::Attack(float dt)
{
	Vector3 playerPos = player->GetWorldPosition();

	sm::Vector3 facingDirection = sm::Vector3(playerPos)
		- sm::Vector3(pOwner->GetWorldPosition());
	facingDirection.Normalize();

	float targetYaw = atan2f(facingDirection.x, facingDirection.z);

	Quat q = Quat::sEulerAngles(Vec3(0.0f, targetYaw, 0.0f));
	PhysicsCommon::physicsSystem->GetBodyInterface().SetRotation(pOwner->GetComponent<Rigidbody>()->GetBodyID(), q, EActivation::Activate);

	timer += dt;
	if (timer >= wholeAttackTime) {
		attacked = false;
		timer = 0.0f;
		endAttack = true;
		return;
	}
	if (attacked) {
		return;
	}
	if (timer < shootTime) {
		return;
	}
	OutputDebugStringA("\nShooting\n");
	attacked = true;

	Vector3 pos = pOwner->GetWorldPosition();
	pos += pOwner->Forward() * 5.0f;
	Node* bullet = PrefabManager::InstantiateBullet(pos.x, pos.y, pos.z, 0.2f);
	bullet->GetComponent<Bullet>()->ignore = pOwner;
	playerPos.y += 2.0f;
	Vector3 dir = playerPos - pos;
	dir.Normalize();
	dir *= bulletSpeed;
	PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(bullet->GetComponent<Rigidbody>()->GetBodyID(), Vec3(dir.x, dir.y, dir.z));

}



void ShootAttack::DrawImGuiControls()
{
	ImGui::Text("Tag: %s", tag.c_str());
	ImGui::InputFloat("Damage", &damage);
	ImGui::InputFloat("Whole Attack Time", &wholeAttackTime);
	ImGui::InputFloat("Shoot Time", &shootTime);
	ImGui::InputFloat("Knockback Force", &knockbackForce);
	ImGui::InputFloat("Attack Timer", &timer);
	ImGui::Checkbox("Attacked", &attacked);
}