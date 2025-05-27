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
	Walking(Node* owner, std::string tag = "Movement");
	virtual ~Walking() = default;
	float maxSpeed = 30.0f;
	float maxForce = 500.0f;
	float rotationLerpFactor = 0.15f;
	Vector3 targetPosition;
	virtual void Follow(DirectX::XMFLOAT3 target, float sp = 1.0f);
	virtual void DrawImGuiControls() override;
	float avoidanceWeight = 17.0f;
	float avoidanceDistance = 8.0f;
	float raycastDistance = 5.0f;
	float raycastWidthOffset = 0.8f;
	float height = 4.0f;
	bool grounded = false;
private:
	Rigidbody* rigidbody;

	Vector3 CalculateAvoidanceForce();
	void GroundCheck();
	bool rightHit, leftHit;
};