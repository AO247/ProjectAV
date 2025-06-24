#include "BoomAttack.h"
#include "Node.h"       
#include "PrefabManager.h"

namespace dx = DirectX;
BoomAttack::BoomAttack(Node* owner, Node* player, std::string tag)
	: Component(owner, std::move(tag)), player(player)
{
}

void BoomAttack::Attack(float dt)
{
	if (attacked)
	{
		pOwner->GetParent()->Destroy();
		return;
	}
	attackRange = 1000.0f;
	pOwner->GetParent()->GetComponent<StateMachine>()->pMovementComponent->Follow(dt, player->GetWorldPosition());
	timer += dt;
	if (timer >= boomTime)
	{
		timer = 0.0f;
		Boom();
		if (pOwner->GetComponent<SoundEffectsPlayer>())
		{
			pOwner->GetComponent<SoundEffectsPlayer>()->Play(0);
		}
	}

}

void BoomAttack::Boom()
{
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i] == nullptr) return;
		if (objects[i]->tag == "PLAYER" || objects[i]->tag == "ENEMY" || objects[i]->tag == "STONE")
		{
			Vector3 direction = objects[i]->GetWorldPosition() -pOwner->GetWorldPosition();
			float length = direction.Length();
			if (length < knockRange)
			{
				float power = knockRange - direction.Length();
				power *= minForce;
				if (power > maxForce)
				{
					power = maxForce;
				}
				direction.Normalize();

				PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(objects[i]->GetComponent<Rigidbody>()->GetBodyID(),
					Vec3(direction.x, direction.y, direction.z) * power);
				if (objects[i]->tag != "STONE")
				{
					objects[i]->GetComponent<Health>()->TakeDamage(1.0f, true);
				}
			}
		}

	}
}

void BoomAttack::OnTriggerStay(const std::vector<Node*> others) {
	objects.clear();
	objects = others;
}


void BoomAttack::DrawImGuiControls()
{
	ImGui::Text("Tag: %s", tag.c_str());
	ImGui::InputFloat("Attack Timer", &timer);
	ImGui::Checkbox("Attacked", &attacked);
	ImGui::InputFloat("BoomTime", &boomTime);
}