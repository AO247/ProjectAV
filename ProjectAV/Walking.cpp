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
	GroundCheck();
	if (VoidCheck() && grounded)
	{
		PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(rigidbody->GetBodyID(), Vec3Arg(0.0f, 0.0f, 0.0f));
			float currentYaw = pOwner->GetLocalRotationEuler().y;
			Vector3 facingDirection = Vector3(targetPosition)
				- Vector3(pOwner->GetWorldPosition());
			facingDirection.Normalize();
			
		if (sp > 1.0f)
		{
			facingDirection -= facingDirection;
			float targetYaw = atan2f(facingDirection.x, facingDirection.z);
			float yawDifference = wrap_angle(targetYaw - currentYaw);
			targetYaw = wrap_angle(currentYaw + yawDifference * rotationLerpFactor * 1.8f);
			Quat q = Quat::sEulerAngles(Vec3(0.0f, targetYaw, 0.0f));
			PhysicsCommon::physicsSystem->GetBodyInterface().SetRotation(rigidbody->GetBodyID(), q, EActivation::Activate);
			PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(rigidbody->GetBodyID(), Vec3Arg(facingDirection.x, facingDirection.y, facingDirection.z) * maxSpeed * 100.0f);
		}
		else 
		{
			float targetYaw = atan2f(facingDirection.x, facingDirection.z);
			float yawDifference = wrap_angle(targetYaw - currentYaw);
			targetYaw = wrap_angle(currentYaw + yawDifference * rotationLerpFactor);
			Quat q = Quat::sEulerAngles(Vec3(0.0f, targetYaw, 0.0f));
			PhysicsCommon::physicsSystem->GetBodyInterface().SetRotation(rigidbody->GetBodyID(), q, EActivation::Activate);
		}

		return;
	}

	PhysicsCommon::physicsSystem->GetBodyInterface().SetFriction(rigidbody->GetBodyID(), 0.0f);


	Vector3 currentPos = pOwner->GetWorldPosition();
	Vec3 currentVelocityJPH = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID());
	Vector3 currentVelocity = { currentVelocityJPH.GetX(), currentVelocityJPH.GetY(), currentVelocityJPH.GetZ() };
	Vector3 desiredDirection = targetPosition - currentPos;
	desiredDirection.Normalize();
	Vector3 desiredVelocity;
	if (!grounded)
	{
		desiredVelocity = desiredDirection * maxSpeed * 0.1f;
	}
	else
	{
		desiredVelocity = desiredDirection * maxSpeed / sp;
	}
	Vector3 steeringForce = desiredVelocity - currentVelocity;

	steeringForce = steeringForce + (CalculateAvoidanceForce());

	float steeringMagnitude = steeringForce.Length();
	if (steeringMagnitude > maxSpeed) {
		steeringForce = (steeringForce / steeringMagnitude) * maxSpeed;
	}

	PhysicsCommon::physicsSystem->GetBodyInterface().AddForce(rigidbody->GetBodyID(), Vec3Arg(steeringForce.x, steeringForce.y, steeringForce.z) * 15.0f);

	currentVelocityJPH = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID());
	currentVelocity = { currentVelocityJPH.GetX(), currentVelocityJPH.GetY(), currentVelocityJPH.GetZ() };
	if (currentVelocity.LengthSquared() > 0.01f)
	{
		Vector3 toTarget = targetPosition - currentPos;
		toTarget.Normalize();
		float dot = currentVelocity.Dot(toTarget);
		float angle = acosf(std::clamp(dot, -1.0f, 1.0f)); // w radianach

		Vector3 facingDirection = currentVelocity;
		facingDirection.Normalize();

		float currentYaw = pOwner->GetLocalRotationEuler().y;
		float targetYaw = atan2f(facingDirection.x, facingDirection.z);

		float yawDifference = wrap_angle(targetYaw - currentYaw);

		targetYaw = wrap_angle(currentYaw + yawDifference * rotationLerpFactor);
		if (angle < maxAllowedAngle) {
			Quat q = Quat::sEulerAngles(Vec3(0.0f, targetYaw, 0.0f));
			PhysicsCommon::physicsSystem->GetBodyInterface().SetRotation(rigidbody->GetBodyID(), q, EActivation::Activate);
		}
	}

	/*if (pOwner->GetComponent<SoundEffectsPlayer>()->isPlaying() == false) {
		pOwner->GetComponent<SoundEffectsPlayer>()->Play(0);
	}*/
	PhysicsCommon::physicsSystem->GetBodyInterface().SetFriction(rigidbody->GetBodyID(), 0.5f);
}
void Walking::GroundCheck()
{
	RRayCast ray = RRayCast(
		RVec3(pOwner->GetWorldPosition().x, pOwner->GetWorldPosition().y, pOwner->GetWorldPosition().z),
		Vec3(0.0f, -(height / 2 + 0.2f), 0.0f)
	);
	RayCastResult result;
	if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(ray, result, SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::GROUND), SpecifiedObjectLayerFilter(Layers::GROUND)))
	{
		grounded = true;
	}
	else
	{
		grounded = false;
	}
}
Vector3 Walking::CalculateAvoidanceForce()
{
	Vector3 avoidanceForce(0.0f, 0.0f, 0.0f);

	Vector3 temporaryDirection = targetPosition - pOwner->GetWorldPosition();
	temporaryDirection.Normalize();

	float radius = 1.0f;

	Vector3 pos = pOwner->GetWorldPosition();
	pos.y += -(height/2.0f) + 0.0f;
	Vector3 forward = temporaryDirection;
	Vector3 right = Vector3(forward.z, 0.0f, -forward.x);
	right.Normalize();

	leftHit = false;
	rightHit = false;
	moreLeft = false;
	moreRight = false;

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
			moreLeft = true;
			avoidanceForce = right * avoidanceWeight * 1.5f;
		}
		else if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayMoreRight, resultMoreRight, SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::WALL), SpecifiedObjectLayerFilter(Layers::WALL)))
		{
			//float distance = Vector3(pos - hitLeft.hitPoint).Length();
			moreRight = true;
			avoidanceForce = -right * avoidanceWeight * 1.5f;
		}
	}

	return avoidanceForce;
}
bool Walking::VoidCheck() 
{
	bool flag = true;
	Vector3 avoidanceForce(0.0f, 0.0f, 0.0f);
	Vector3 temporaryDirection = targetPosition - pOwner->GetWorldPosition();
	temporaryDirection.Normalize();

	float radius = 1.0f;

	Vector3 pos = pOwner->GetWorldPosition();

	Vector3 centerOrigin = pos + temporaryDirection * 3.0f;

	RRayCast rayLeft = RRayCast(
		RVec3(centerOrigin.x, centerOrigin.y, centerOrigin.z),
		RVec3(0.0f, -10.0f, 0.0f)
	);
	RayCastResult resultLeft;
	if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayLeft, resultLeft, SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::GROUND), SpecifiedObjectLayerFilter(Layers::GROUND)))
	{
		flag = false;
	}

	voidNear = flag;
	return flag;
}


void Walking::DrawImGuiControls()
{
	Vec3 velocity = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID());
	float vel = velocity.Length();
	ImGui::Text("Tag: %s", tag.c_str());
	ImGui::InputFloat("Max Speed", &maxSpeed);
	ImGui::InputFloat("Avoidance Weight", &avoidanceWeight);
	ImGui::InputFloat("Avoidance Distance", &avoidanceDistance);
	ImGui::Checkbox("Grounded", &grounded);
	ImGui::Checkbox("Lefy Hit", &leftHit);
	ImGui::Checkbox("Right Hit", &rightHit);
	ImGui::Checkbox("More Left", &moreLeft);
	ImGui::Checkbox("More Right", &moreRight);
	ImGui::InputFloat("Velocity", &vel);
	ImGui::Checkbox("Void", &voidNear);


}