#pragma once

#include "Component.h"
#include "Window.h"
#include <DirectXMath.h>
#include "Rigidbody.h"

class Node;

class Walking : public Component
{
public:
	Walking(Node* owner, std::string tag = "MOVEMENT");
	virtual ~Walking() = default;
	float maxSpeed = 60.0f;
	float rotationLerpFactor = 0.15f;
	Vector3 targetPosition;
	virtual void Follow(float dt, DirectX::XMFLOAT3 target, float sp = 1.0f);
	virtual void DrawImGuiControls() override;
	float avoidanceWeight = 20.0f;
	float avoidanceDistance = 15.0f;
	float raycastWidthOffset = 0.8f;
	float height = 8.0f;

	float jumpForce = 1000.0f;
	float jumpRange = 40.0f;

	bool grounded = false;
	bool voidNear = false;
	bool canJump = false;
	float jumpTimer = 0.0f;
	Vector3 lastIslandPos = { 0.0f, 0.0f, 0.0f };

private:
	Rigidbody* rigidbody;

	Vector3 CalculateAvoidanceForce();
	void GroundCheck();
	bool VoidCheck();
	bool Jump();
	bool rightHit, leftHit, moreLeft, moreRight;
	float maxAllowedAngle = DirectX::XMConvertToRadians(120.0f);
};