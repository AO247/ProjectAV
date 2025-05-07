#pragma once

#include "Component.h"
#include "Window.h" // Needs access to Window for input
#include <DirectXMath.h>
#include "Rigidbody.h"

// Forward declare Node to avoid circular include if necessary,
// but including Node.h is often fine here.
class App;
class Node;

class PlayerController : public Component
{
public:
	PlayerController(Node* owner, Window& window);

	virtual void Update(float dt) override;
	DirectX::XMFLOAT2 GetRotation() const;
	void SetRotation(float pitch, float yaw) noexcept;
	virtual void DrawImGuiControls() override;


	float moveSpeed = 12.0f;
	float jumpForce = 60.0f;

	uint32_t sound1;

private:
	void HandleMovementInput(float dt);
	void HandleMouseLookInput();
	bool jumped = false;
	bool godMode = false;
	Rigidbody* rigidbody; // Rigidbody for physics interactions
	Window& wnd;

	float pitch = 0.0f;
	float yaw = 0.0f;
	float airMultiplier = 1.2f;


	float rotationSpeed = 0.004f;
	static constexpr float pitchLimit = DirectX::XM_PI / 2.0f * 0.995f;

};