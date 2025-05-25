#include "Walking.h"
#include "Node.h"       
#include "Window.h" 
#include "CMath.h"
#include <DirectXMath.h>
#include <algorithm>
#include "CapsuleCollider.h"
#include "SoundEffectsPlayer.h"
#include "DebugLine.h"

namespace dx = DirectX;
Walking::Walking(Node* owner, std::string tag)
	: Component(owner, std::move(tag))
{
	rigidbody = owner->GetComponent<Rigidbody>();
}
void Walking::Follow(DirectX::XMFLOAT3 targetPos, float sp)
{
    if (!rigidbody) { 
        return;
    }
	targetPosition = targetPos;
    Vector3 currentPos = pOwner->GetWorldPosition(); 
	Vec3 currentVelocityJPH = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID());
	Vector3 currentVelocity = { currentVelocityJPH.GetX(), currentVelocityJPH.GetY(), currentVelocityJPH.GetZ()};
    Vector3 desiredDirection = targetPosition - currentPos;

    desiredDirection.Normalize();
    Vector3 desiredVelocity = desiredDirection * maxSpeed / sp;

    Vector3 steeringForce = desiredVelocity - currentVelocity;

	steeringForce = steeringForce + (CalculateAvoidanceForce());

    float steeringMagnitude = steeringForce.Length();
    if (steeringMagnitude > maxForce) {
        steeringForce = (steeringForce / steeringMagnitude) * maxForce; 
    }

	PhysicsCommon::physicsSystem->GetBodyInterface().AddForce(rigidbody->GetBodyID(), Vec3Arg(steeringForce.x, steeringForce.y, steeringForce.z) * 10.0f);
	currentVelocityJPH = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID());
	currentVelocity = { currentVelocityJPH.GetX(), currentVelocityJPH.GetY(), currentVelocityJPH.GetZ() };
	if (currentVelocity.LengthSquared() > 0.01f)
    {
        Vector3 facingDirection = currentVelocity; 
        facingDirection.Normalize();


		float currentYaw = pOwner->GetLocalRotationEuler().y;
		float targetYaw = atan2f(facingDirection.x, facingDirection.z);

		float yawDifference = wrap_angle(targetYaw - currentYaw);
		
		//targetYaw = wrap_angle(currentYaw + yawDifference * rotationLerpFactor);

        pOwner->SetLocalRotation({ 0.0f, targetYaw, 0.0f });
    }

	if (pOwner->GetComponent<SoundEffectsPlayer>()->isPlaying() == false) {
		pOwner->GetComponent<SoundEffectsPlayer>()->Play(0);
	}
}

Vector3 Walking::CalculateAvoidanceForce()
{
	Vector3 avoidanceForce(0.0f, 0.0f, 0.0f);

	Vector3 previousRotation = pOwner->GetLocalRotationEuler();
	//pOwner->TranslateLocal({ 0.0f, 1.0f, 0.0f });
	Vector3 temporaryDirection = targetPosition - pOwner->GetWorldPosition();
	temporaryDirection.Normalize();
	float targetYaw = atan2f(temporaryDirection.x, temporaryDirection.z);
	pOwner->SetLocalRotation({ 0.0f, targetYaw, 0.0f });

	float radius = 1.0f;

	Vector3 pos = pOwner->GetWorldPosition();
	pos.y += (-height/2.0f) + 1.0f;
	Vector3 forward = pOwner->Forward();
	Vector3 right = pOwner->Right();

	leftHit = false;
	rightHit = false;

	Vector3 centerOrigin = pos + forward;
	Vector3 leftOrigin = centerOrigin - right * radius;
	Vector3 rightOrigin = centerOrigin + right * radius;

	Vector3 centerDir = forward;
	
	Vector3 leftDir = (forward - right * 0.5f); leftDir.Normalize(); 

	Vector3 rightDir = (forward + right * 0.5f); rightDir.Normalize();

	centerDir *= avoidanceDistance;
	leftDir *= avoidanceDistance;
	rightDir *= avoidanceDistance;

	float targetDistance = Vector3(pos - targetPosition).Length();
	RRayCast rayLeft = RRayCast(
		RVec3(leftOrigin.x, leftOrigin.y, leftOrigin.z),
		RVec3(centerDir.x, centerDir.y, centerDir.z)
	);
	RayCastResult resultLeft;
	if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayLeft, resultLeft, SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::WALL), SpecifiedObjectLayerFilter(Layers::WALL)))
	{
		leftHit = true;
		//float distance = Vector3(pos - hitLeft.hitPoint).Length();
		//if(distance < targetDistance)
		avoidanceForce = right * avoidanceWeight;
	}


	RRayCast rayRight = RRayCast(
		RVec3(rightOrigin.x, rightOrigin.y, rightOrigin.z),
		RVec3(centerDir.x, centerDir.y, centerDir.z)
	);
	RayCastResult resultRight;
	if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayRight, resultRight, SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::WALL), SpecifiedObjectLayerFilter(Layers::WALL)))
	{
		rightHit = true;
		//float distance = Vector3(pos - hitLeft.hitPoint).Length();
		//if(distance < targetDistance)
		avoidanceForce = -right * avoidanceWeight;
	}


	if (leftHit && rightHit) {
		RRayCast rayMoreLeft = RRayCast(
			RVec3(leftOrigin.x, leftOrigin.y, leftOrigin.z),
			RVec3(leftDir.x, leftDir.y, leftDir.z)
		);
		RRayCast rayMoreRight = RRayCast(
			RVec3(leftOrigin.x, leftOrigin.y, leftOrigin.z),
			RVec3(rightDir.x, rightDir.y, rightDir.z)
		);
		RayCastResult resultMoreLeft;
		RayCastResult resultMoreRight;

		if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayMoreLeft, resultMoreLeft, SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::WALL), SpecifiedObjectLayerFilter(Layers::WALL)))
		{
			//float distance = Vector3(pos - hitLeft.hitPoint).Length();
			//if(distance < targetDistance)
			avoidanceForce = -right * avoidanceWeight;
		}
		else if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayMoreRight, resultMoreRight, SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::WALL), SpecifiedObjectLayerFilter(Layers::WALL)))
		{
			//float distance = Vector3(pos - hitLeft.hitPoint).Length();
			avoidanceForce = right * avoidanceWeight;
		}
	}

	pOwner->SetLocalRotation(previousRotation);
	//pOwner->TranslateLocal({ 0.0f, -1.0f, 0.0f });

	return avoidanceForce;

}

void Walking::DrawImGuiControls()
{
   ImGui::Text("Tag: %s", tag.c_str());
   ImGui::InputFloat("Max Speed", &maxSpeed);
   ImGui::InputFloat("Max Force", &maxForce);
   ImGui::InputFloat("Avoidance Weight", &avoidanceWeight);
   ImGui::InputFloat("Avoidance Distance", &avoidanceDistance);

}