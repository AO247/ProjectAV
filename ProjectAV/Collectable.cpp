#include "Collectable.h"
#include <string>
Collectable::Collectable(Node* owner, float targetY) : Component(owner), targetY(targetY)
{
	rigidbody = pOwner->GetComponent<Rigidbody>();
	// Wylosuj wartoœæ floty z przedzia³u od 1 do 3
	speed = rand() % 400 + 200;
	speed = speed / 100.0f;
	basePosition = pOwner->GetLocalPosition();
	dirx = rand() % 4 - 2;
	dirz = rand() % 4 - 2;
	if (dirx == 0) dirx = 2;
	if (dirz == 0) dirz = 2;
}
void Collectable::Update(float dt)
{
	if (basePosition.y - targetY > 0.1f)
	{
		basePosition.y -= 0.03f;
	}
	timer += dt;
	float offsetX = dirx * std::cos(timer) * speed * 0.5f;
	float offsetY = std::sin(timer) * speed * 0.3f;
	float offsetZ = dirz * std::sin(timer * 0.5f) * speed * 0.3f;
	Vector3 position = basePosition;
	position.x += offsetX;
	position.y += offsetY;
	position.z += offsetZ;

	pOwner->SetLocalPosition(position);
}

void Collectable::OnTriggerEnter(Node* object)
{
	if (object == nullptr) return;
	if (object->tag == "PLAYER")
	{
		if (health)
		{
			object->GetComponent<Health>()->TakeDamage(-1);
		}
		else
		{
			object->GetComponent<PlayerController>()->abilitySlot3->GetComponent<Ability>()->killsCount++;
		}
		pOwner->Destroy();

	}
}


void Collectable::DrawImGuiControls()
{
}
