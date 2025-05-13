#include "ShootAttack.h"
#include "Node.h"       


namespace dx = DirectX;
ShootAttack::ShootAttack(Node* owner, std::string tag)
	: Component(owner, std::move(tag))
{
}

void ShootAttack::Attack(float dt)
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
	if (timer < shootTime) {
		return;
	}
	//OutputDebugStringA("\nShooting\n");
	attacked = true;
	// here we will create bullet
}



void ShootAttack::DrawImGuiControls()
{
	ImGui::Text("Tag: %s", tag.c_str());
	ImGui::InputFloat("Damage", &damage);
	ImGui::InputFloat("Whole Attack Time", &wholeAttackTime);
	ImGui::InputFloat("Shoot Time", &shootTime);
	ImGui::InputFloat("Knockback Force", &knockbackForce);
	ImGui::InputFloat("Attack Timer", &timer);
	ImGui::Checkbox("Attacked", &attacked);
}