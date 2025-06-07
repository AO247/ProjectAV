#pragma once

#include "Ability.h"
#include "Window.h" // Needs access to Window for input
#include <DirectXMath.h>
#include "Rigidbody.h"
#include "SoundEffectsPlayer.h"

// Forward declare Node to avoid circular include if necessary,
// but including Node.h is often fine here.
class Node;

class Ability2 : public Ability
{
public:
	Ability2(Node* owner, Window& window, Node* camera);
	virtual ~Ability2() = default;

	virtual void Update(float dt) override;
	virtual void DrawImGuiControls() override;
	void OnTriggerEnter(Node* other) override;
	void OnTriggerExit(Node* other) override;
	void Pressed() override;
	void Released() override;
	float cooldown = 0.5f; // Cooldown for ability 1
	float force = 300.0f;
	bool abilityReady = true; // Is the ability ready to be used?
private:
	void KeyboardInput();
	void Cooldowns(float dt);
	void Positioning();
	float cooldownTimer = 0.0f;
};