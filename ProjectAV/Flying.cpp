#include "Flying.h"
#include "Node.h"       
#include "Window.h" 
#include "CMath.h"
#include <DirectXMath.h>
#include <algorithm>
#include "SoundEffectsPlayer.h"
#include "DebugLine.h"

namespace dx = DirectX;
Flying::Flying(Node* owner, std::string tag)
	: Component(owner, std::move(tag))
{
	rigidbody = owner->GetComponent<Rigidbody>();
	PhysicsCommon::physicsSystem->GetBodyInterface().SetGravityFactor(rigidbody->GetBodyID(), 0.0f);
	PhysicsCommon::physicsSystem->GetBodyInterface().SetFriction(rigidbody->GetBodyID(), 0.0f);
}
void Flying::Follow(float dt, DirectX::XMFLOAT3 targetPos, float sp)
{
	if (!rigidbody) {
		return;
	}
	targetPosition = targetPos;
	if (sp > 1.0f)
	{
		if (VoidCheck() || !grounded)
		{
			targetPosition = lastIslandPos;
		}

	}

	Vector3 currentPos = pOwner->GetWorldPosition();
	Vec3 currentVelocityJPH = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID());
	Vector3 currentVelocity = { currentVelocityJPH.GetX(), currentVelocityJPH.GetY(), currentVelocityJPH.GetZ() };
	Vector3 desiredDirection = targetPosition - currentPos;
	desiredDirection.y = 0.0f;
	desiredDirection.Normalize();

	Vector3 desiredVelocity = desiredDirection * maxSpeed / sp;


	Vector3 steeringForce = desiredVelocity - currentVelocity;

	steeringForce = steeringForce + (CalculateAvoidanceForce());

	float steeringMagnitude = steeringForce.Length();
	if (steeringMagnitude > maxSpeed) {
		steeringForce = (steeringForce / steeringMagnitude) * maxSpeed;
	}
	if (!goingUp)
	{
		steeringForce += HeightCalculate();
	}
	               
	PhysicsCommon::physicsSystem->GetBodyInterface().AddForce(rigidbody->GetBodyID(), Vec3Arg(steeringForce.x, steeringForce.y, steeringForce.z) * 1500.0f * dt);

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
}

Vector3 Flying::CalculateAvoidanceForce()
{
	Vector3 avoidanceForce(0.0f, 0.0f, 0.0f);

	Vector3 temporaryDirection = targetPosition - pOwner->GetWorldPosition();
	temporaryDirection.Normalize();

	float radius = 1.0f;

	Vector3 pos = pOwner->GetWorldPosition();
	pos.y += -(height / 2.0f) + 0.0f;
	Vector3 forward = temporaryDirection;
	Vector3 right = Vector3(forward.z, 0.0f, -forward.x);
	right.Normalize();

	leftHit = false;
	rightHit = false;
	moreLeft = false;
	moreRight = false;
	goingUp = false;

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


	RRayCast ray = RRayCast(
		RVec3(centerOrigin.x, centerOrigin.y, centerOrigin.z),
		RVec3(centerDir.x * 2.0f, centerDir.y * 2.0f, centerDir.z * 2.0f)
	);
	RayCastResult result;
	if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(ray, result, SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::GROUND), SpecifiedObjectLayerFilter(Layers::GROUND)))
	{
		leftHit = true;
		avoidanceForce = Vector3(0.0f, 1.0f, 0.0f) * avoidanceWeight;
		goingUp = true;
		return avoidanceForce;
	}



	RRayCast rayLeft = RRayCast(
		RVec3(leftOrigin.x, leftOrigin.y, leftOrigin.z),
		RVec3(centerDir.x, centerDir.y, centerDir.z)
	);
	RayCastResult resultLeft;
	if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayLeft, resultLeft, SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::WALL), SpecifiedObjectLayerFilter(Layers::WALL)))
	{
		leftHit = true;
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
			moreLeft = true;
			avoidanceForce = right * avoidanceWeight * 1.5f;
		}
		else if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayMoreRight, resultMoreRight, SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::WALL), SpecifiedObjectLayerFilter(Layers::WALL)))
		{
			moreRight = true;
			avoidanceForce = -right * avoidanceWeight * 1.5f;
		}
	}

	return avoidanceForce;

}

Vector3 Flying::HeightCalculate()
{
	Vector3 force(0.0f, 0.0f, 0.0f);
	Vector3 temporaryDirection = targetPosition - pOwner->GetWorldPosition();
	temporaryDirection.Normalize();

	float radius = 1.0f;

	Vector3 pos = pOwner->GetWorldPosition();

	RRayCast ray = RRayCast(
		RVec3(pos.x, pos.y, pos.z),
		RVec3(0.0f, -100, 0.0f)
	);
	RayCastResult result;
	if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(ray, result, SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::GROUND), SpecifiedObjectLayerFilter(Layers::GROUND)))
	{
		Vec3 position = ray.mOrigin + ray.mDirection * result.mFraction;
		pos.y = position.GetY() + flyingHeight;
		force = pos - pOwner->GetWorldPosition();
		Vec3 tymPos = PhysicsCommon::physicsSystem->GetBodyInterface().GetPosition(result.mBodyID);
		lastIslandPos = { tymPos.GetX(), tymPos.GetY(), tymPos.GetZ() };
		grounded = true;
	}
	else 
	{
		grounded = false;
	}


	force.x = 0.0f;
	force.z = 0.0f;
	force *= heightAdjustmentWeight;

	static float bounceTime = 0.0f;
	bounceTime += 0.01f; 

	force.y += std::sin(bounceTime) * heightBounceWeight;

	return force;
}
bool Flying::VoidCheck()
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
		RVec3(0.0f, -50.0f, 0.0f)
	);
	RayCastResult resultLeft;
	if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayLeft, resultLeft, SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::GROUND), SpecifiedObjectLayerFilter(Layers::GROUND)))
	{
		flag = false;
	}

	return flag;
}

void Flying::DrawImGuiControls()
{
	ImGui::Text("Tag: %s", tag.c_str());
	ImGui::InputFloat("Max Speed", &maxSpeed);
	ImGui::InputFloat("Max Force", &maxForce);
	ImGui::InputFloat("Avoidance Weight", &avoidanceWeight);
	ImGui::InputFloat("Avoidance Distance", &avoidanceDistance);
	ImGui::InputFloat("Height Adjustment Weight", &heightAdjustmentWeight);
	ImGui::InputFloat("Height Bounce Weight", &heightBounceWeight);
	ImGui::InputFloat("Flying Height", &flyingHeight);

}