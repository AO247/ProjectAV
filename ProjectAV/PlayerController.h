#pragma once

#include "Component.h"
#include "Window.h" // Needs access to Window for input
#include <DirectXMath.h>
#include "Rigidbody.h"
#include "SoundEffectsPlayer.h"

// Forward declare Node to avoid circular include if necessary,
// but including Node.h is often fine here.
class Node;

class PlayerController : public Component
{
public:
	PlayerController(Node* owner, Window& window);
	virtual ~PlayerController() = default;

	virtual void Update(float dt) override;
	virtual void DrawImGuiControls() override;

	float moveSpeed = 23.0f;
	//float jumpForce = 300.0f;
	//float dashForce = 600.0f;
	float jumpForce = 500.0f;
	float dashForce = 1000.0f;
	float height = 4.0f;
	float dashCooldown = 1.0f;
	float ability1Cooldown = 1.0f;
	float ability2Cooldown = 1.0f;
	bool alive = true;
private:
	Window& wnd;
	Rigidbody* rigidbody;
	Node* camera;
	Node* ability1;
	Node* ability2;
	bool jumped = false;
	bool grounded = false;
	bool canDash = true;
	float airMultiplier = 1.6f;

	Vector3 moveDirection;
	Vector3 dashDirection;
	float dashCooldownTimer = 0.0f;
	float ability1CooldownTimer = 0.0f;
	float ability2CooldownTimer = 0.0f;
	float dashTimer = 0.0f;
	bool dashed = false;
	bool doubleJumped = false;
	bool ability1Ready = true;
	bool ability2Ready = true;

	void KeyboardInput();
	void MovePlayer();
	void SpeedControl();
	void GroundCheck();
	void Jump();
	void Dash();
	void Ability1();
	void Ability2();
	void Cooldowns(float dt);
	void Positioning();

};