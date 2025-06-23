////////////////////////////////
// Evolve of ability 2        //
// Throw up for some duration //
////////////////////////////////
#pragma once

#include "Ability.h"
#include "Window.h"
#include <DirectXMath.h>
#include "Rigidbody.h"
#include "SoundEffectsPlayer.h"

class Node;

class Ability5 : public Ability
{
public:
	Ability5(Node* owner, Window& window, Node* camera);
	virtual ~Ability5() = default;

	virtual void Update(float dt) override;
	virtual void DrawImGuiControls() override;
	bool Pressed() override;
	void Released() override;
	float cooldown = 1.2f;
	float duration = 3.0f;
	float timeToChange = 0.0f;
	bool stop = true;

	float force = 900.0f;
	bool abilityReady = true;

	Node* rightHandNormal = nullptr;
	Node* rightHandAbility = nullptr;
private:
	void KeyboardInput();
	void Cooldowns(float dt);
	void Positioning();
	float cooldownTimer = 0.0f;
};
