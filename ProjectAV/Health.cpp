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
	bool hitted = false;
	if (tank)
	{
		if (heavy)
		{
			hitted = true;
			pOwner->GetComponent<StateMachine>()->Stop(0.1f);
			currentHealth -= damage;
		}
		else 
		{
			hitted = true;
			pOwner->GetComponent<StateMachine>()->Stun(1.6f);
		}

	}
	else
	{	
		if (pOwner->tag == "PLAYER" && damage > 0)
		{
			if (pOwner->GetComponent<SoundEffectsPlayer>())
			{
				float randSound = (rand() % 6 + 6);
				pOwner->GetComponent<SoundEffectsPlayer>()->Play(randSound, 1.0f, false);
			}
		}
		else if (pOwner->tag == "ENEMY")
		{
			hitted = true;
			pOwner->GetComponent<StateMachine>()->Stop(0.1f);
		}
		currentHealth -= damage;

	}

	if (isFire && !fireType)
	{
		currentHealth -= damage;
		hitted = true;
		pOwner->GetComponent<StateMachine>()->Stop(0.1f);
	}

	if (hitted && pOwner->tag == "ENEMY")
	{
		// tutaj dŸwiêk otrzymania obra¿eñ 
		// animacja otrzymania obra¿eñ
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
