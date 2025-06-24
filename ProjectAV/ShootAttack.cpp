#include "ShootAttack.h"
#include "Node.h"       
#include "PrefabManager.h"

namespace dx = DirectX;
ShootAttack::ShootAttack(Node* owner, Node* player, std::string tag)
	: Component(owner, std::move(tag)), player(player)
{
}

void ShootAttack::Attack(float dt)
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
		// miejsce na animacje
	}
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
	attacked = true;

	Vector3 pos = pOwner->GetWorldPosition();
	pos += pOwner->Forward() * 5.0f;
	Node* bullet = PrefabManager::InstantiateBullet(Vector3(pos.x, pos.y, pos.z), 0.2f);
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