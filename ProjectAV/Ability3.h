#pragma once

#include "Component.h"
#include "Window.h" // Needs access to Window for input
#include <DirectXMath.h>
#include "Rigidbody.h"
#include "SoundEffectsPlayer.h"

// Forward declare Node to avoid circular include if necessary,
// but including Node.h is often fine here.
class Node;

class Ability3 : public Component
{
public:
	Ability3(Node* owner, Window& window, Node* camera);
	virtual ~Ability3() = default;

	virtual void Update(float dt) override;
	virtual void DrawImGuiControls() override;
	void OnTriggerEnter(Node* other) override;
	void OnTriggerExit(Node* other) override;
	void Active();
	void Activated();
	float cooldown = 1.5f; // Cooldown for ability 1
	float force = 800.0f;
	float duration = 1.0f;
	float timer = 0.0f;
	bool abilityReady = true; // Is the ability ready to be used?
private:
	Window& wnd;
	Node* camera = nullptr;
	void KeyboardInput();
	void Cooldowns(float dt);
	void Positioning();
	std::vector<Node*> objects;
	float cooldownTimer = 0.0f;
};