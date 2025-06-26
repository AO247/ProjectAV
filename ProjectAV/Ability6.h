////////////////////////////////
// Evolve of ability 1        //
// Pull item then throw       //
////////////////////////////////
#pragma once

#include "Ability.h"
#include "Window.h"
#include <DirectXMath.h>
#include "Rigidbody.h"
#include "SoundEffectsPlayer.h"
#include "AnimationComponent.h"

class Node;

class Ability6 : public Ability
{
public:
	Ability6(Node* owner, Window& window, Node* camera);
	virtual ~Ability6() = default;

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

	AnimationComponent* leftHand = nullptr;
	Node* selectedNode = nullptr;
	Ability* baseAbility = nullptr;

	Node* pullingParticles = nullptr;
	Node* holdParticles = nullptr;
	Node* holdSmokeParticles = nullptr;
private:
	Node* player = nullptr;
	void KeyboardInput();
	void Cooldowns(float dt);
	void Positioning();
	void Pulling(float dt);
	void PullingParticlesPositioning();
	float cooldownTimer = 0.0f;
};