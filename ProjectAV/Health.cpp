#include "Health.h"
#include <string>
#include "Components.h"
Health::Health(Node* owner, float maxHelth) 
	: Component(owner), maxHealth(maxHelth)
{
	currentHealth = maxHealth;
}

void Health::DrawImGuiControls()
{
	ImGui::Text("Health: %f", currentHealth);
	ImGui::Text("Max Health: %f", maxHealth);
}

void Health::TakeDamage(float damage)
{
	currentHealth -= damage;
	if (currentHealth <= 0.0f)
	{
		if (pOwner->tag != "PLAYER") {
			GetOwner()->Destroy();
		}
		else {
			pOwner->GetComponent<PlayerController>()->alive = false;
		}
	}
}
