#include "Bullet.h"
#include <string>
Bullet::Bullet(Node* owner) : Component(owner)
{
	rigidbody = owner->GetComponent<Rigidbody>();
}
void Bullet::Update(float dt)
{
	timer += dt;
	if (timer >= lifeTime) {
		pOwner->Destroy();
	}
}

void Bullet::OnCollisionEnter(Node* object)
{
	if (object == ignore || object->tag == "TRIGGER") return;
	if (object->GetComponent<Health>())
	{
		if(object->tag == "PLAYER") object->GetComponent<Health>()->TakeDamage(damage);
	}
	pOwner->Destroy();
}


void Bullet::DrawImGuiControls()
{
	/*ImGui::InputFloat("Move Speed", &moveSpeed);
	ImGui::Checkbox("Jumped", &jumped);*/

}
