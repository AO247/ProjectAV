#pragma once

#include "Component.h"
#include "Window.h" // Needs access to Window for input
#include <DirectXMath.h>
#include "Rigidbody.h"

// Forward declare Node to avoid circular include if necessary,
// but including Node.h is often fine here.
class Node;

class Walking : public Component
{
public:
	Walking(Node* owner);
	std::string tag;

	float maxSpeed;
	float maxForce;
	float rotationLerpFactor;
	Vector3 targetPosition;
	virtual void Follow(DirectX::XMFLOAT3 target);
	virtual void DrawImGuiControls() override;
private:
	Rigidbody* rigidbody; // Rigidbody for physics interactions

	float pitch = 0.0f;
	float yaw = 0.0f;
	float airMultiplier = 1.2f;

	float rotationSpeed = 0.004f;
	static constexpr float pitchLimit = DirectX::XM_PI / 2.0f * 0.995f;
};