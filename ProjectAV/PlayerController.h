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

	float moveSpeed = 23.0f;
	float jumpForce = 30.0f;
	float secondJumpForce = 20.0f;
	float dashForce = 70.0f;
	float height = 4.0f;
	float dashCooldown = 1.0f;
	bool alive = true;
	bool grounded = false;

	Node* abilitySlot1;
	Node* abilitySlot2;
	Node* abilitySlot3;
	Node* camera;
	Rigidbody* rigidbody;

private:
	Window& wnd;

	bool jumped = false;
	bool canDash = true;
	float airMultiplier = 1.6f;

	Vector3 moveDirection;
	Vector3 dashDirection;
	float dashCooldownTimer = 0.0f;
	float dashTimer = 0.0f;
	bool dashed = false;
	bool doubleJumped = false;
	float autoJumpRange = 1.0f;
	void KeyboardInput();
	void MovePlayer(float dt);
	void SpeedControl();
	void PlayerGroundCheck();
	void AutoJump();
	void Jump();
	void Dash();
	void Cooldowns(float dt);
	void Positioning();

};