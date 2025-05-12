#include "Throwable.h"
#include <string>
Throwable::Throwable(Node* owner) : Component(owner)
{
	rigidbody = owner->GetComponent<Rigidbody>();
}
void Throwable::Update(float dt)
{
	if (rigidbody->GetVelocity().Length() > 40.0f)
	{
		OnTriggerEnter();
	}
}

void Throwable::OnTriggerEnter()
{
	std::vector<Collider*> cols = damageArea->GetTriggerEnter();

	if (cols.size() > 0)
	{
		for (Collider* col : cols)
		{
			if (col->GetOwner()->tag == "Enemy")
			{
				//OutputDebugStringA(("\nSTONE VELOCITY: " + std::to_string(rigidbody->GetVelocity().Length())).c_str());
				col->GetOwner()->Destroy();
			}
		}
	}
}


void Throwable::DrawImGuiControls()
{
	/*ImGui::InputFloat("Move Speed", &moveSpeed);
	ImGui::Checkbox("Jumped", &jumped);*/

}
