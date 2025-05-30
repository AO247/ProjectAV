#pragma once

#include "Component.h"
#include "Window.h" // Needs access to Window for input
#include <DirectXMath.h>
#include "Rigidbody.h"

// Forward declare Node to avoid circular include if necessary,
// but including Node.h is often fine here.
class Node;

class Flying : public Component
{
public:
	Flying(Node* owner, std::string tag = "Movement");
	virtual ~Flying() = default;
	float maxSpeed = 40.0f;
	float maxForce = 200.0f;
	float height = 5.0f;
	float rotationLerpFactor = 0.08f;
	Vector3 targetPosition;
	virtual void Follow(DirectX::XMFLOAT3 target, float sp = 1.0f);
	virtual void DrawImGuiControls() override;
	float avoidanceWeight = 17.0f;
	float avoidanceDistance = 8.0f;
	float raycastDistance = 5.0f;
	float raycastWidthOffset = 0.8f;
private:
	Rigidbody* rigidbody;

	Vector3 CalculateAvoidanceForce();
	bool rightHit, leftHit;
};