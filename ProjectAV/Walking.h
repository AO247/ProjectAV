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
	Walking(Node* owner, std::string tag = "MOVEMENT");
	virtual ~Walking() = default;
	float maxSpeed = 60.0f;
	float rotationLerpFactor = 0.15f;
	Vector3 targetPosition;
	virtual void Follow(DirectX::XMFLOAT3 target, float sp = 1.0f);
	virtual void DrawImGuiControls() override;
	float avoidanceWeight = 30.0f;
	float avoidanceDistance = 15.0f;
	float raycastWidthOffset = 0.8f;
	float height = 8.0f;
	bool grounded = false;
	bool voidNear = false;
private:
	Rigidbody* rigidbody;

	Vector3 CalculateAvoidanceForce();
	void GroundCheck();
	bool VoidCheck();
	bool rightHit, leftHit, moreLeft, moreRight;
	float maxAllowedAngle = DirectX::XMConvertToRadians(120.0f);
};