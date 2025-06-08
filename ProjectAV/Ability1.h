#pragma once

#include "Ability.h"
#include "Window.h" // Needs access to Window for input
#include <DirectXMath.h>
#include "Rigidbody.h"
#include "SoundEffectsPlayer.h"

// Forward declare Node to avoid circular include if necessary,
// but including Node.h is often fine here.
class Node;

class Ability1 : public Ability
{
public:
	Ability1(Node* owner, Window& window, Node* camera);
	virtual ~Ability1() = default;


	virtual void Update(float dt) override;
	virtual void DrawImGuiControls() override;
	void OnTriggerEnter(Node* other) override;
	void OnTriggerExit(Node* other) override;
	void Pressed() override;
	void Released() override;
	float cooldown = 1.2f; // Cooldown for ability 1
	float timeToChange = 0.0f;
	float force = 600.0f;
	bool abilityReady = true; // Is the ability ready to be used?
	bool stop = true;
	Node* leftHandNormal = nullptr;
	Node* leftHandAbility = nullptr;

private:
	/*Window& wnd;
	Node* camera = nullptr;*/
	void KeyboardInput();
	void Cooldowns(float dt);
	void Positioning();
	float cooldownTimer = 0.0f;
};