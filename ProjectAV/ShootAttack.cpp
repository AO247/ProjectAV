#include "ShootAttack.h"
#include "Node.h"       
#include "PrefabManager.h"

namespace dx = DirectX;
ShootAttack::ShootAttack(Node* owner, std::string tag)
	: Component(owner, std::move(tag))
{
	player = pOwner->GetParent()->FindFirstChildByTag("PLAYER");
	attackRange = 45.0f;
}

void ShootAttack::Attack(float dt)
{
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
	// here we will create bullet
	Vector3 pos = pOwner->GetWorldPosition();
	pos += pOwner->Forward() * 5.0f;
	Node* bullet = PrefabManager::InstantiateBullet(pos.x, pos.y, pos.z, 0.2f);
	bullet->GetComponent<Bullet>()->ignore = pOwner;
	Vector3 playerPos = player->GetWorldPosition();
	playerPos.y += 2.0f;
	Vector3 dir = playerPos - pos;
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