#include "Collectable.h"
#include <string>
Collectable::Collectable(Node* owner) : Component(owner)
{
	rigidbody = pOwner->GetComponent<Rigidbody>();
}

void Collectable::OnTriggerEnter(Node* object)
{
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
