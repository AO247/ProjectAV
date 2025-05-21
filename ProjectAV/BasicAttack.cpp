#include "BasicAttack.h"
#include "Node.h"       


namespace dx = DirectX;
BasicAttack::BasicAttack(Node* owner, std::string tag)
	: Component(owner, std::move(tag))
{
}

void BasicAttack::Attack(float dt)
{
	timer += dt;
	if (timer >= wholeAttackTime) {
		attacked = false;
		timer = 0.0f;
		endAttack = true;
		return;
	}
	if (attacked) {
		return;
	}
	if (timer < startDmgTime) {
		return;
	}
	else if (timer > stopDmgTime) {
		return;
	}
	//OnTriggerCheck();
	//OutputDebugStringA("\nNormal Attack\n");
}

//void BasicAttack::OnTriggerCheck()
//{
//	if (damageArea == nullptr) {
//		return;
//	}
//	std::vector<Collider*> cols = damageArea->GetTriggerStay();
//
//	if (cols.size() > 0)
//	{
//		for (Collider* col : cols)
//		{
//			if (col->GetIsTrigger()) continue;
//			if (col->GetOwner()->GetComponent<Health>() && col->GetOwner()->tag == "Player")
//			{
//				col->GetOwner()->GetComponent<Health>()->TakeDamage(1.0f);
//				Rigidbody* rb = col->GetOwner()->GetComponent<Rigidbody>();
//				Vector3 pos(pOwner->GetWorldPosition().x, col->GetOwner()->GetWorldPosition().y - 0.5f, pOwner->GetWorldPosition().z);
//				Vector3 knockDirection = col->GetOwner()->GetWorldPosition() - pos;
//				knockDirection.Normalize();
//				rb->SetVelocity(Vector3(0.0f, 0.0f, 0.0f));
//				rb->AddForce(knockDirection * knockbackForce * 100.0f);
//				attacked = true;
//			}
//		}
//	}
//}


void BasicAttack::DrawImGuiControls()
{
	ImGui::Text("Tag: %s", tag.c_str());
	ImGui::InputFloat("Damage", &damage);
	ImGui::InputFloat("Whole Attack Time", &wholeAttackTime);
	ImGui::InputFloat("Start Damage Time", &startDmgTime);
	ImGui::InputFloat("Stop Damage Time", &stopDmgTime);
	ImGui::InputFloat("Knockback Force", &knockbackForce);
	ImGui::InputFloat("Attack Timer", &timer);
	ImGui::Checkbox("Attacked", &attacked);
}