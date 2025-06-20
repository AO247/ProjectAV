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

void Health::TakeDamage(float damage, bool heavy, bool isFire)
{
	if (tank)
	{
		if (heavy)
		{
			currentHealth -= damage;
		}
		else {
			pOwner->GetComponent<StateMachine>()->Stun(1.6f);
		}
	}
	else
	{
		currentHealth -= damage;
	}

	if (isFire && !fireType)
	{
		currentHealth -= damage;
	}

	if (currentHealth > maxHealth)
	{
		currentHealth = maxHealth;
	}
	if (currentHealth <= 0.0f)
	{
		if (pOwner->tag != "PLAYER") {
			pOwner->GetComponent<StateMachine>()->eatedPills = true;
		}
		else {
			pOwner->GetComponent<PlayerController>()->alive = false;
		}
	}
}
