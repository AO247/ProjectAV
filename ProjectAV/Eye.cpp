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
		pOwner->GetComponent<Health>()->TakeDamage(damage); // 0 - rotateAtak, 1- fireball, 2 - 4fireball, 3 - stanie, 4 - tpose, 5 - walk
		pOwner->GetComponent<StateMachine>()->Stun(1.5f);
	}
}


void Eye::DrawImGuiControls()
{

}
