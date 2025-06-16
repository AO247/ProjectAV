//////////////////////////
// Classic push ability //
//////////////////////////
#pragma once
#include "Ability.h"
#include "Window.h"
#include <DirectXMath.h>
#include "Rigidbody.h"
#include "SoundEffectsPlayer.h"

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
	float cooldown = 1.2f;
	float timeToChange = 0.0f;
	float force = 600.0f;
	bool abilityReady = true;
	bool stop = true;
	Node* leftHandNormal = nullptr;
	Node* leftHandAbility = nullptr;

private:
	void KeyboardInput();
	void Cooldowns(float dt);
	void Positioning();
	float cooldownTimer = 0.0f;
};