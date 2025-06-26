#pragma once

#include "Component.h"
#include "Window.h"
#include <DirectXMath.h>
#include "Rigidbody.h"
#include "SoundEffectsPlayer.h"

class Node;

class PlayerController : public Component
{
public:
	PlayerController(Node* owner, Window& window);
	virtual ~PlayerController() = default;

	virtual void Update(float dt) override;
	virtual void DrawImGuiControls() override;

	float acceleration = 7.0f;
	float deceleration = 25.0f;
	float airControl = 0.5f;
	float groundDamping = 0.0001;
	float airDamping = 0.04f;
	float slowTime = 0.0f;
	float slowPower = 0.5f;

	float maxSpeed = 25.0f;

	float jumpForce = 25.0f;
	float secondJumpForce = 25.0f;
	float dashForce = 70.0f;
	float height = 6.0f;
	float dashCooldown = 1.5f;

	float stepSoundInterval = 0.38f;
	float stepSoundTimer = 0.0f;

	bool alive = true;
	bool grounded = false;
	bool evolvedDash = false;
	bool enableFallPush = false;

	std::vector<Node*> objects;
	//std::optional<Keyboard::Event> e;
	Keyboard::Event key;
	Node* abilitySlot1;
	Node* abilitySlot2;
	Node* abilitySlot3;
	Node* camera;

	Node* leftHand;
	Node* rightHand;


	Rigidbody* rigidbody;
	Vector3 moveDirection;

private:
	Window& wnd;

	bool jumped = false;
	bool canDash = true;
	float airMultiplier = 1.1f;

	Vector3 dashDirection;
	float dashCooldownTimer = 0.0f;
	float dashTimer = 0.0f;
	bool dashed = false;
	bool doubleJumped = false;
	float autoJumpRange = 1.0f;
	float autoJumpHeight = 2.0f;
	void KeyboardInput();
	void MovePlayer(float dt);
	void SpeedControl(float dt);
	void PlayerGroundCheck();
	void AutoJump();
	void Jump();
	void Dash();
	void Cooldowns(float dt);
	void Positioning();

};