#pragma once

#include "Component.h"
#include "Window.h"
#include <DirectXMath.h>
#include "Rigidbody.h"

class Node;

class Flying : public Component
{
public:
	Flying(Node* owner, std::string tag = "MOVEMENT");
	virtual ~Flying() = default;
	float maxSpeed = 18.0f;
	float maxForce = 200.0f;
	float flyingHeight = 12.0f;
	float height = 3.0f;
	float rotationLerpFactor = 0.08f;

	float heightAdjustmentWeight = 15.0f;
	float heightBounceWeight = 8.0f;

	Vector3 targetPosition;
	virtual void Follow(float dt, DirectX::XMFLOAT3 target, float sp = 1.0f);
	virtual void DrawImGuiControls() override;
	float avoidanceWeight = 17.0f;
	float avoidanceDistance = 8.0f;
	float raycastDistance = 5.0f;
	float raycastWidthOffset = 0.8f;

	Vector3 lastIslandPos = { 0.0f, 0.0f, 0.0f };
	bool grounded = false;
	bool goingUp = false;
private:
	Rigidbody* rigidbody;
	Vector3 HeightCalculate();
	Vector3 CalculateAvoidanceForce();
	bool VoidCheck();
	bool rightHit, leftHit, moreLeft, moreRight;
	float maxAllowedAngle = DirectX::XMConvertToRadians(120.0f);

};