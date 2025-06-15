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

void Health::TakeDamage(float damage, bool heavy)
{
	if (tank)
	{
		if (heavy)
		{
			currentHealth -= damage;
		}
		else {
			pOwner->GetComponent<StateMachine>()->RequestStateChange(StateType::STUN);
		}
	}
	else
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
			pOwner->GetComponent<StateMachine>()->Die();
		}
		else {
			pOwner->GetComponent<PlayerController>()->alive = false;
		}
	}
}
