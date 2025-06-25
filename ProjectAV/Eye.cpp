#include "Eye.h"
#include <string>
Eye::Eye(Node* owner) : Component(owner)
{
	rigidbody = owner->GetComponent<Rigidbody>();
}
void Eye::Update(float dt)
{
	timer += dt;
}

void Eye::OnTriggerEnter(Node* object)
{
	if (object == nullptr) return;
	if (object->tag == "STONE")
	{
		pOwner->GetComponent<Health>()->TakeDamage(damage);
	}
}


void Eye::DrawImGuiControls()
{

}
