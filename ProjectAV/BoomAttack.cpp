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
		attacked = true;
	}

}
void BoomAttack::OnTriggerStay(Node* object)
{
	if (!attacked) return;
	if (object == nullptr) return;
	if (object->tag == "PLAYER" || object->tag == "ENEMY" || object->tag == "STONE")
	{
		Vector3 currentPos = pOwner->GetWorldPosition();
		Vector3 objPos = object->GetWorldPosition();
		Vector3 direction = objPos - currentPos;
		float power = knockRange - direction.Length();
		direction.Normalize();

		PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(object->GetComponent<Rigidbody>()->GetBodyID(), Vec3(direction.x, direction.y, direction.z) * power * 500.0f);

	}
	if (object->tag == "PLAYER" || object->tag == "ENEMY")
	{
		object->GetComponent<Health>()->TakeDamage(1.0f, true);
	}

}


void BoomAttack::DrawImGuiControls()
{
	ImGui::Text("Tag: %s", tag.c_str());
	ImGui::InputFloat("Attack Timer", &timer);
	ImGui::Checkbox("Attacked", &attacked);
	ImGui::InputFloat("BoomTime", &boomTime);
}