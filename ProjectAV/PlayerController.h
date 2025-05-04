#pragma once

#include "Component.h"
#include "Window.h" // Needs access to Window for input
#include <DirectXMath.h>
#include "Rigidbody.h"

// Forward declare Node to avoid circular include if necessary,
// but including Node.h is often fine here.
class Node;

class PlayerController : public Component
{
public:
	PlayerController(Node* owner, Window& window);
	virtual void Update(float dt) override;
	virtual void DrawImGuiControls() override;

	float moveSpeed = 12.0f;
	float jumpForce = 60.0f;
	float dashForce = 1000.0f;

private:
	Window& wnd;
	Rigidbody* rigidbody;
	Node* camera;

	bool jumped = false;
	bool dashed = false;

	float airMultiplier = 1.2f;

	Vector3 moveDirection;
	Vector3 dashDirection;

	void KeyboardInput();
	void MovePlayer();
	void SpeedControl();
	void Jump();
	void Dash();

};