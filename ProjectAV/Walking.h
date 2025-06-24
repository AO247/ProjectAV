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
	float maxSpeed = 80.0f;
	float rotationLerpFactor = 0.04f;
	Vector3 targetPosition;
	virtual void Follow(float dt, DirectX::XMFLOAT3 target, float sp = 1.0f);
	virtual void DrawImGuiControls() override;
	virtual bool GroundCheck() override;

	float voidCheckRange = 4.0f;
	float avoidanceWeight = 30.0f;
	float avoidanceDistance = 8.0f;
	float raycastWidthOffset = 0.8f;
	float height = 8.0f;
	float radius = 1.0f;

	float jumpForce = 1000.0f;
	float jumpRange = 40.0f;

	bool grounded = false;
	bool voidNear = false;
	bool canJump = false;
	bool rotateToTarget = false;

	float timerStanding = 0.0f;
	float timerForChangedDirection = 0.0f;
	float jumpTimer = 0.0f;
	float avoidObjectDistance = 2.0f;
	Vector3 lastIslandPos = { 0.0f, 0.0f, 0.0f };

	float stepSoundInterval = 4.0f; 
	float stepSoundTimer = 0.0f;
	float IdleSoundTimer = 0.0f;

private:
	Rigidbody* rigidbody;

	Vector3 CalculateAvoidanceForce();
	bool VoidCheck();
	void AutoJump();
	bool Jump();
	bool rightHit, leftHit, moreLeft, moreRight;
	float maxAllowedAngle = DirectX::XMConvertToRadians(120.0f);
};