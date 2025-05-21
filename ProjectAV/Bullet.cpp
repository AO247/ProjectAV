#include "Bullet.h"
#include <string>
Bullet::Bullet(Node* owner) : Component(owner)
{
	rigidbody = owner->GetComponent<Rigidbody>();
}
void Bullet::Update(float dt)
{
	OnTriggerEnter();
}

void Bullet::OnTriggerEnter()
{
	/*std::vector<Collider*> cols = damageArea->GetTriggerEnter();

	if (cols.size() > 0)
	{
		for (Collider* col : cols)
		{
			if (col->GetIsTrigger()) continue;
			if (col->GetOwner()->tag == "Player")
			{
				col->GetOwner()->GetComponent<Health>()->TakeDamage(damage);
			}
			pOwner->Destroy();
		}
	}*/
}


void Bullet::DrawImGuiControls()
{
	/*ImGui::InputFloat("Move Speed", &moveSpeed);
	ImGui::Checkbox("Jumped", &jumped);*/

}
