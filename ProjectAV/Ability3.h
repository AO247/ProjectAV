#pragma once

#include "Ability.h"
#include "Window.h" // Needs access to Window for input
#include <DirectXMath.h>
#include "Rigidbody.h"
#include "SoundEffectsPlayer.h"

// Forward declare Node to avoid circular include if necessary,
// but including Node.h is often fine here.
class Node;

class Ability3 : public Ability
{
public:
	Ability3(Node* owner, Window& window, Node* camera);
	virtual ~Ability3() = default;

	virtual void Update(float dt) override;
	virtual void DrawImGuiControls() override;
	void OnTriggerEnter(Node* other) override;
	void OnTriggerExit(Node* other) override;
	void Pressed() override;
	void Released() override;
	void Activated();
	float timer = 0.0f;
	
	float cooldown = 1.5f;
	float duration = 1.0f;
	float maxDistance = 40.0f;
	float minForce = 50.0f;
	float maxForce = 800.0f;

	bool abilityReady = true;
private:
	void KeyboardInput();
	void Cooldowns(float dt);
	void Positioning();
	float cooldownTimer = 0.0f;
};