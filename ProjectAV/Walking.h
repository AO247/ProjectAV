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
	float avoidanceWeight = 150.0f;
	float raycastDistance = 5.0f;
	float raycastWidthOffset = 0.8f;
private:
	Rigidbody* rigidbody;

	Vector3 CalculateAvoidanceForce();
	bool rightHit, leftHit, centerHit;
};