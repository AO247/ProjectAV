////////////////////////////////
// Evolve of ability 1        //
// Pick item then pick a spot //
////////////////////////////////
#pragma once

#include "Ability.h"
#include "Window.h"
#include <DirectXMath.h>
#include "Rigidbody.h"
#include "SoundEffectsPlayer.h"

class Node;

class Ability4 : public Ability
{
public:
	Ability4(Node* owner, Window& window, Node* camera);
	virtual ~Ability4() = default;

	virtual void Update(float dt) override;
	virtual void DrawImGuiControls() override;
	bool Pressed() override;
	void Released() override;

	float cooldown = 1.5f;
	float timeToChange = 0.0f;
	float pressedTime = 0.0f;
	bool stop = true;

	float force = 600.0f;
	Vector3	cameraRotation = Vector3::Zero;
	bool isPressed = false;
	bool abilityReady = true;

	Node* leftHandNormal = nullptr;
	Node* leftHandAbility = nullptr;
	Node* selectedNode = nullptr;
	Ability* baseAbility = nullptr;

	Node* selectionParticles = nullptr;
private:
	Node* player = nullptr;
	void KeyboardInput();
	void Cooldowns(float dt);
	void Positioning();
	float cooldownTimer = 0.0f;
};