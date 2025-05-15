#include "Throwable.h"

Throwable::Throwable(Node* owner) : Component(owner)
{
	rigidbody = owner->GetComponent<Rigidbody>();
}
void Throwable::Update(float dt)
{
	/*if (rigidbody->GetVelocity().Length() > 0.0f)
	{
		OnTriggerEnter();
	}*/
}

void Throwable::OnTriggerEnter()
{
	/*std::vector<Collider*> cols = damageArea->GetTriggerEnter();

	if (cols.size() > 0)
	{
		for (Collider* col : cols)
		{
			if (col->GetOwner()->tag == "Enemy")
			{
				OutputDebugStringA("\nHit Enemy\n");
			}
		}
	}*/
}


void Throwable::DrawImGuiControls()
{
	/*ImGui::InputFloat("Move Speed", &moveSpeed);
	ImGui::Checkbox("Jumped", &jumped);*/

}
