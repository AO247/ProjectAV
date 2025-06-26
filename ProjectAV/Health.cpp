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

void Health::TakeDamage(float damage, bool heavy, bool isFire, bool extraHeavy)
{
	StateMachine* statemachine = pOwner->GetComponent <StateMachine>();


	bool hitted = false;
	if (!boss)
	{
		if (damage == 0) // bullet
		{
			hitted = true;
			pOwner->GetComponent<StateMachine>()->Stun(1.6f);
		}
		if (tank)
		{
			if (heavy || extraHeavy)
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

		if (isFire && !fireType && pOwner->tag == "ENEMY")
		{
			currentHealth -= damage;
			hitted = true;
			pOwner->GetComponent<StateMachine>()->Stop(0.1f);
		}
		else if (isFire && pOwner->tag == "PLAYER")
		{
			currentHealth -= damage;
		}

		if (hitted && pOwner->tag == "ENEMY")
		{
			// tutaj dŸwiêk otrzymania obra¿eñ 
			// animacja otrzymania obra¿eñ

			if (statemachine->enemyType == EnemyType::BASIC) {
				pOwner->GetComponent<AnimationComponent>()->PlayAnimation(3);	//basic

			}
			if (statemachine->enemyType == EnemyType::RANGED) {
				pOwner->GetComponent<AnimationComponent>()->PlayAnimation(2);	//ranged

			}
			if (statemachine->enemyType == EnemyType::TANK) {
				pOwner->GetComponent<AnimationComponent>()->PlayAnimation(3);	//tank

			}
			if (statemachine->enemyType == EnemyType::EXPLOSIVE) {
				pOwner->GetComponent<AnimationComponent>()->PlayAnimation(8);	//explosive

			}
			if (statemachine->enemyType == EnemyType::FRENZY) {
				pOwner->GetComponent<AnimationComponent>()->PlayAnimation(7);	//fast
			}
			if (statemachine->enemyType == EnemyType::FLYING) {
				pOwner->GetComponent<AnimationComponent>()->PlayAnimation(4);	//flying
			}
			if (statemachine->enemyType == EnemyType::MAGE) {
				pOwner->GetComponent<AnimationComponent>()->PlayAnimation(6);	//mage
			}
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
	else
	{
		pOwner->GetComponent<AnimationComponent>()->PlayAnimation(4, false);
		if (heavy)
			currentHealth -= damage * 2;
		else if (isFire)
			currentHealth -= damage;
		else {
			currentHealth -= damage;
		}
		if (currentHealth <= maxHealth * 0.6f)
		{

		}
	}



}
