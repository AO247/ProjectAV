//////////////////////////////
// Classic throw up ability //
//////////////////////////////
#pragma once

#include "Ability.h"
#include "Window.h"
#include <DirectXMath.h>
#include "Rigidbody.h"
#include "SoundEffectsPlayer.h"

class Node;

class Ability2 : public Ability
{
public:
	Ability2(Node* owner, Window& window, Node* camera);
	virtual ~Ability2() = default;

	virtual void Update(float dt) override;
	virtual void DrawImGuiControls() override;
	void OnTriggerStay(const std::vector<Node*> others) override;

	bool Pressed() override;
	void Released() override;
	float cooldown = 1.2f;
	float timeToChange = 0.0f;
	bool stop = true;

	float force = 300.0f;
	bool abilityReady = true;

	Node* rightHandNormal = nullptr;
	Node* rightHandAbility = nullptr;
private:
	void KeyboardInput();
	void Cooldowns(float dt);
	void Positioning();
	float cooldownTimer = 0.0f;
};