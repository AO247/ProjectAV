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
	if (currentHealth > maxHealth)
	{
		currentHealth = maxHealth;
	}
	if (damage < 0)
	{
		pOwner->GetComponent<PlayerController>()->abilitySlot3->GetComponent<Ability>()->killsCount++;
	}
	if (currentHealth <= 0.0f)
	{
		if (pOwner->tag != "PLAYER") {
			pOwner->Destroy();
			pOwner->GetRoot()->FindFirstChildByTag("PLAYER")->GetComponent<Health>()->TakeDamage(-1);
		}
		else {
			pOwner->GetComponent<PlayerController>()->alive = false;
		}
	}
}
