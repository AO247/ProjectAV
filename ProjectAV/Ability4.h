#pragma once

#include "Ability.h"
#include "Window.h" // Needs access to Window for input
#include <DirectXMath.h>
#include "Rigidbody.h"
#include "SoundEffectsPlayer.h"

// Forward declare Node to avoid circular include if necessary,
// but including Node.h is often fine here.
class Node;

class Ability4 : public Ability
{
public:
	Ability4(Node* owner, Window& window, Node* camera);
	virtual ~Ability4() = default;

	virtual void Update(float dt) override;
	virtual void DrawImGuiControls() override;
	void OnTriggerEnter(Node* other) override;
	void OnTriggerExit(Node* other) override;
	void Pressed() override;
	void Released() override;
	void Activated();

	float cooldown = 1.0f;
	float force = 600.0f;
	Vector3	cameraRotation = Vector3::Zero;
	bool isPressed = false;
	bool abilityReady = true;
private:
	Node* player = nullptr;
	void KeyboardInput();
	void Cooldowns(float dt);
	void Positioning();
	float cooldownTimer = 0.0f;
};