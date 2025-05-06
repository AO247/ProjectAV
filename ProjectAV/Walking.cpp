#include "Walking.h"
#include "Node.h"       
#include "Window.h" 
#include "CMath.h"
#include <DirectXMath.h>
#include <algorithm>
#include "CapsuleCollider.h"
namespace dx = DirectX;
Walking::Walking(Node* owner)
	: Component(owner) 
{
	rigidbody = owner->GetComponent<Rigidbody>();
	tag = "Movement";
    maxSpeed = 20.0f;
    maxForce = 200.0f;
    rotationLerpFactor = 0.1f;
}
void Walking::Follow(DirectX::XMFLOAT3 targetPosition)
{
    if (!rigidbody || rigidbody->GetIsStatic()) { 
        return;
    }

    Vector3 targetPos(targetPosition);
    Vector3 currentPos = rigidbody->GetPosition(); 
    Vector3 currentVelocity = rigidbody->GetVelocity();

    Vector3 desiredDirection = targetPos - currentPos;

    desiredDirection.Normalize();
    Vector3 desiredVelocity = desiredDirection * maxSpeed;

    Vector3 steeringForce = desiredVelocity - currentVelocity;

	steeringForce = steeringForce + (CalculateAvoidanceForce() * avoidanceWeight);

    float steeringMagnitude = steeringForce.Length();
    if (steeringMagnitude > maxForce) {
        steeringForce = (steeringForce / steeringMagnitude) * maxForce; 
    }

	rigidbody->AddForce(steeringForce * 10.0f);

	currentVelocity = rigidbody->GetVelocity();
    if (currentVelocity.LengthSquared() > 0.01f) 
    {
        Vector3 facingDirection = currentVelocity; 
        facingDirection.Normalize();

        float targetYaw = atan2f(facingDirection.x, facingDirection.z);

        GetOwner()->SetLocalRotation({ 0.0f, targetYaw, 0.0f });
    }
}

Vector3 Walking::CalculateAvoidanceForce()
{
	Vector3 avoidanceForce(0.0f, 0.0f, 0.0f);
	float radius = GetOwner()->GetComponent<CapsuleCollider>()->GetRadius();

	Vector3 pos = GetOwner()->GetWorldPosition();
	Vector3 forward = GetOwner()->Forward();
	Vector3 right = GetOwner()->Right();

	leftHit = false;
	rightHit = false;
	centerHit = false;

	Vector3 centerOrigin = pos + forward;
	Vector3 leftOrigin = centerOrigin - right * radius;
	Vector3 rightOrigin = centerOrigin + right * radius;

	Vector3 centerDir = forward;
	
	// Vector3 leftDir = (forward - right * 0.3f); leftDir.Normalize(); 
	// Vector3 rightDir = (forward + right * 0.3f); rightDir.Normalize();

	RaycastData hitLeft = Raycast::Cast(leftOrigin, centerDir);
	RaycastData hitRight = Raycast::Cast(rightOrigin, centerDir);
	RaycastData hitCenter = Raycast::Cast(centerOrigin, centerDir);

	float targetDistance = Vector3(pos - targetPosition).Length();

	if (hitLeft.hitCollider != nullptr && hitLeft.hitCollider->GetOwner()->tag == "Wall" && Vector3(hitLeft.hitPoint - pos).Length() < targetDistance) {
		leftHit = true;

		avoidanceForce = right * maxForce;
	}

	if (hitRight.hitCollider != nullptr && hitRight.hitCollider->GetOwner()->tag == "Wall"  && Vector3(hitRight.hitPoint - pos).Length() < targetDistance) {
		rightHit = true;
		avoidanceForce = -right * maxForce;
	}

	if (!leftHit && !rightHit && hitCenter.hitCollider != nullptr && hitCenter.hitCollider->GetOwner()->tag == "Wall" && Vector3(hitCenter.hitPoint - pos).Length() < targetDistance) {
		centerHit = true;
		avoidanceForce = -right * maxForce * 1.5f;
	}

	return avoidanceForce;

}

void Walking::DrawImGuiControls()
{
   ImGui::Text("Player Controller Properties:");
   ImGui::Text("Tag: %s", tag.c_str());
   ImGui::InputFloat("Max Speed", &maxSpeed);
   ImGui::InputFloat("Max Force", &maxForce);
   ImGui::Checkbox("Right", &rightHit);
   ImGui::Checkbox("Left", &leftHit);
   ImGui::Checkbox("Center", &centerHit);

}