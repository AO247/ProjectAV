/////////////////////////
// Evolve of ability 2 //
// BLACK HOLE          //
/////////////////////////
#pragma once

#include "Ability.h"
#include "Window.h"
#include <DirectXMath.h>
#include "Rigidbody.h"
#include "SoundEffectsPlayer.h"
#include "AnimationComponent.h"

class Node;

class Ability3 : public Ability
{
public:
	Ability3(Node* owner, Window& window, Node* camera);
	virtual ~Ability3() = default;

	virtual void Update(float dt) override;
	virtual void DrawImGuiControls() override;
	void OnTriggerStay(const std::vector<Node*> others) override;
	void OnCollisionEnter(Node* other) override;
	bool Pressed() override;
	void Released() override;
	void Activated();
	float timer = 0.0f;
	
	bool isPressed = false;
	float duration = 1.3f;
	float maxDistance = 40.0f;
	float minForce = 50.0f;
	float maxForce = 800.0f;
	float animTimer = 0.0f;
	bool released = false;
	bool activated = false;
	bool readyToActive = false;
	Vec3 pos = Vec3(0.0f, 0.0f, 0.0f);
	AnimationComponent* leftHand = nullptr;
	AnimationComponent* rightHand = nullptr;

	float activeSoundTimer = 0.0f;
	float activeSoundInterval = 1.6f;
	float throwSoundTimer = 0.0f;
	float throwSoundInterval = 1.5f;
private:
	void KeyboardInput();
	void Positioning();
	float cooldownTimer = 0.0f;
};