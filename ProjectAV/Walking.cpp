#include "Walking.h"
#include "Node.h"       
#include "Window.h" 
#include "CMath.h"
#include <DirectXMath.h>
#include <algorithm>
#include "SoundEffectsPlayer.h"
#include "DebugLine.h"
#include "Components.h"

namespace dx = DirectX;
Walking::Walking(Node* owner, std::string tag)
	: Component(owner, std::move(tag))
{
	rigidbody = owner->GetComponent<Rigidbody>();
	PhysicsCommon::physicsSystem->GetBodyInterface().SetFriction(rigidbody->GetBodyID(), 0.01f);

}
void Walking::Follow(float dt, DirectX::XMFLOAT3 targetPos, float sp)
{
	if (!rigidbody) {
		return;
	}
	if (jumpTimer > 0.0f)
	{
		jumpTimer -= dt;
	}
	targetPosition = targetPos;
	Vector3 currentPos = pOwner->GetWorldPosition();
	Vec3 currentVelocityJPH = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID());

	if (GroundCheck())
	{
		canAttack = true;
	}
	if (VoidCheck() && grounded)
	{
		if (Jump())
		{
			return;
		}
		if (sp > 1.0f)
		{
			targetPosition = lastIslandPos;
			Vector3 facingDirection = Vector3(targetPosition) - Vector3(pOwner->GetWorldPosition());
			facingDirection.y = 0.0f;
			facingDirection.Normalize();
			PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(rigidbody->GetBodyID(), Vec3(facingDirection.x, facingDirection.y, facingDirection.z) * 10.0f);
			timerForChangedDirection = 0.5f;
		}
		else 
		{
			float currentYaw = pOwner->GetLocalRotationEuler().y;
			Vector3 facingDirection = Vector3(targetPosition)
				- Vector3(pOwner->GetWorldPosition());
			facingDirection.Normalize();
			Vec3 velocity = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID());
			PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(rigidbody->GetBodyID(), Vec3(0.0f, velocity.GetY(), 0.0f));
			float targetYaw = atan2f(facingDirection.x, facingDirection.z);
			float yawDifference = wrap_angle(targetYaw - currentYaw);
			targetYaw = wrap_angle(currentYaw + yawDifference * rotationLerpFactor);
			Quat q = Quat::sEulerAngles(Vec3(0.0f, targetYaw, 0.0f));
			PhysicsCommon::physicsSystem->GetBodyInterface().SetRotation(rigidbody->GetBodyID(), q, EActivation::Activate);
			//tutaj animacja stania

			return;
		}

	}

	stepSoundTimer -= dt;
	IdleSoundTimer -= dt;

	// tutaj dŸwiêk i animacja chodzenia


	//AutoJump();
	if (timerForChangedDirection > 0.0f)
	{
		timerForChangedDirection -= dt;
		targetPosition = lastIslandPos;
	}

	Vector3 currentVelocity = { currentVelocityJPH.GetX(), currentVelocityJPH.GetY(), currentVelocityJPH.GetZ() };

	if (pOwner->GetComponent<SoundEffectsPlayer>() && currentVelocity.LengthSquared() > 0.1f && grounded)
	{
		if (IdleSoundTimer <= 0.0f)
		{
			//Idle sounds
			float randSound = (rand() % 4);
			pOwner->GetComponent<SoundEffectsPlayer>()->Play(randSound, 1.0f);

			IdleSoundTimer = IdleSoundInterval;
		}
		if (stepSoundTimer <= 0.0f) {
			//Steps sounds
			int randSound = rand() % 6 + 7;
			pOwner->GetComponent<SoundEffectsPlayer>()->PlayAdvanced(randSound, 0.4, false, 1.0f, 5.0f, 40.0f, 2.0f, true);
			stepSoundTimer = stepSoundInterval;
		}
	}

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

	Vector3 avoidanceForce = CalculateAvoidanceForce();
	if(moreLeft || moreRight)
	{
		steeringForce = avoidanceForce;
	}
	else
	{
		steeringForce += avoidanceForce;
	}

	float steeringMagnitude = steeringForce.Length();
	//if (steeringMagnitude > maxSpeed) {
		steeringForce = (steeringForce / steeringMagnitude) * maxSpeed;
	//}

	PhysicsCommon::physicsSystem->GetBodyInterface().AddForce(rigidbody->GetBodyID(), Vec3Arg(steeringForce.x, 0.0f, steeringForce.z) * 1000.0f * dt);


	currentVelocityJPH = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID());
	currentVelocity = { currentVelocityJPH.GetX(), currentVelocityJPH.GetY(), currentVelocityJPH.GetZ() };
	if (currentVelocity.LengthSquared() > 0.01f)
	{
		Vector3 toTarget = targetPosition - currentPos;
		toTarget.Normalize();
		float dot = currentVelocity.Dot(toTarget);
		float angle = acosf(std::clamp(dot, -1.0f, 1.0f));

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

	//PhysicsCommon::physicsSystem->GetBodyInterface().SetFriction(rigidbody->GetBodyID(), 0.5f);
}
bool Walking::GroundCheck()
{
	grounded = false;
	canAttack = false;
	Vector3 pos = pOwner->GetWorldPosition();
	Vec3 center = {pos.x, pos.y, pos.z};
	Vec3 forwardDir = Vec3(pOwner->Forward().x, 0.0f, pOwner->Forward().z);
	Vec3 rightDir = Vec3(pOwner->Right().x, 0.0f, pOwner->Right().x);
	
	Vec3 front = center * forwardDir * radius;
	Vec3 back = center - forwardDir * radius;
	Vec3 right = center + rightDir * radius;
	Vec3 left = center - rightDir * radius;

	front.SetY(center.GetY());
	back.SetY(center.GetY());
	left.SetY(center.GetY());
	right.SetY(center.GetY());



	Vec3 direction = Vec3(0.0f, -(height / 2 + 0.2f), 0.0f);

	RRayCast rayCenter = RRayCast(
		center,
		direction
	);
	RayCastResult resultCenter;
	if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayCenter, resultCenter,
		MultipleBroadPhaseLayerFilter({ BroadPhaseLayers::GROUND, BroadPhaseLayers::WALL }),
		MultipleObjectLayerFilter({ Layers::GROUND, Layers::WALL})))
	{
		grounded = true;
		canAttack = true;
		Node* no = reinterpret_cast<Node*>(PhysicsCommon::physicsSystem->GetBodyInterface().GetUserData(resultCenter.mBodyID));
		if (no != nullptr)
		{
			if (no->tag == "GROUND")
			{
				lastIslandPos = no->GetWorldPosition();
			}
			return true;
		}
	}

	RRayCast rayFront = RRayCast(
		front,
		direction
	);
	RayCastResult resultFront;
	if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayFront, resultFront,
		MultipleBroadPhaseLayerFilter({ BroadPhaseLayers::GROUND, BroadPhaseLayers::WALL }),
		MultipleObjectLayerFilter({ Layers::GROUND, Layers::WALL})))
	{
		grounded = true;
		canAttack = true;
		return true;
	}

	RRayCast rayBack = RRayCast(
		back,
		direction
	);
	RayCastResult resultBack;
	if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayBack, resultBack,
		MultipleBroadPhaseLayerFilter({ BroadPhaseLayers::GROUND, BroadPhaseLayers::WALL }),
		MultipleObjectLayerFilter({ Layers::GROUND, Layers::WALL})))
	{
		grounded = true;
		canAttack = true;
		return true;
	}

	RRayCast rayRight = RRayCast(
		right,
		direction
	);
	RayCastResult resultRight;
	if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayRight, resultRight,
		MultipleBroadPhaseLayerFilter({ BroadPhaseLayers::GROUND, BroadPhaseLayers::WALL }),
		MultipleObjectLayerFilter({ Layers::GROUND, Layers::WALL})))
	{
		grounded = true;
		canAttack = true;
		return true;
	}

	RRayCast rayLeft = RRayCast(
		left,
		direction
	);
	RayCastResult resultLeft;
	if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayLeft, resultLeft,
		MultipleBroadPhaseLayerFilter({ BroadPhaseLayers::GROUND, BroadPhaseLayers::WALL }),
		MultipleObjectLayerFilter({ Layers::GROUND, Layers::WALL})))
	{
		grounded = true;
		canAttack = true;
		return true;
	}



	return false;
}


Vector3 Walking::CalculateAvoidanceForce()
{
	Vector3 avoidanceForce(0.0f, 0.0f, 0.0f);

	Vector3 temporaryDirection = targetPosition - pOwner->GetWorldPosition();
	temporaryDirection.y = 0.0f;
	temporaryDirection.Normalize();


	Vector3 pos = pOwner->GetWorldPosition();
	pos.y += -(height/2.0f) + 0.3f;
	Vector3 forward = temporaryDirection;
	forward.y = 0.0f;
	Vector3 right = Vector3(forward.z, 0.0f, -forward.x);
	right.Normalize();
	forward.Normalize();

	leftHit = false;
	rightHit = false;
	moreLeft = false;
	moreRight = false;

	Vector3 centerOrigin = pos + forward * radius;
	Vector3 leftOrigin = pos - right * radius;
	Vector3 rightOrigin = pos + right * radius;

	Vector3 centerDir = forward;
	
	Vector3 leftDir = (forward - right * 0.5f); leftDir.Normalize(); 

	Vector3 rightDir = (forward + right * 0.5f); rightDir.Normalize();

	centerDir *= avoidanceDistance;
	leftDir *= avoidanceDistance;
	rightDir *= avoidanceDistance;


	float targetDistance = Vector3(pOwner->GetWorldPosition() - targetPosition).Length();
	RRayCast rayLeft = RRayCast(
		RVec3(leftOrigin.x, leftOrigin.y, leftOrigin.z),
		RVec3(centerDir.x, centerDir.y, centerDir.z)
	);

	RayCastResult resultLeft;
	if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayLeft, resultLeft,
		IgnoreMultipleBroadPhaseLayerFilter({ BroadPhaseLayers::ENEMY, BroadPhaseLayers::TRIGGER, BroadPhaseLayers::PLAYER }),
		IgnoreMultipleObjectLayerFilter({ Layers::ENEMY, Layers::TRIGGER, Layers::PLAYER })))
	{
		Vec3 hitPosition = rayLeft.mOrigin + rayLeft.mDirection * resultLeft.mFraction;
		Vector3 hitPos = Vector3(hitPosition.GetX(), hitPosition.GetY(), hitPosition.GetZ());

		float distance = Vector3(pOwner->GetWorldPosition() - hitPos).Length();
		if (distance < targetDistance)
		{
			leftHit = true;
			avoidanceForce = right * avoidanceWeight * (8.0f - distance);
		}
	}


	RRayCast rayRight = RRayCast(
		RVec3(rightOrigin.x, rightOrigin.y, rightOrigin.z),
		RVec3(centerDir.x, centerDir.y, centerDir.z)
	);
	RayCastResult resultRight;
	if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayRight, resultRight, 
		IgnoreMultipleBroadPhaseLayerFilter({ BroadPhaseLayers::ENEMY, BroadPhaseLayers::TRIGGER, BroadPhaseLayers::PLAYER }),
		IgnoreMultipleObjectLayerFilter({ Layers::ENEMY, Layers::TRIGGER, Layers::PLAYER })))
	{
		Vec3 hitPosition = rayRight.mOrigin + rayRight.mDirection * resultRight.mFraction;
		Vector3 hitPos = Vector3(hitPosition.GetX(), hitPosition.GetY(), hitPosition.GetZ());

		float distance = Vector3(pOwner->GetWorldPosition() - hitPos).Length();
		if (distance < targetDistance)
		{
			rightHit = true;
			avoidanceForce = -right * avoidanceWeight * (8.0f - distance);
			//OutputDebugStringA(("\nRight Hit into:" + no->GetName()).c_str());
		}
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

		if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayMoreLeft, resultMoreLeft, 
			IgnoreMultipleBroadPhaseLayerFilter({ BroadPhaseLayers::ENEMY, BroadPhaseLayers::TRIGGER, BroadPhaseLayers::PLAYER }),
			IgnoreMultipleObjectLayerFilter({ Layers::ENEMY, Layers::TRIGGER, Layers::PLAYER })))
		{
			Vec3 hitPosition = rayMoreLeft.mOrigin + rayMoreLeft.mDirection * resultMoreLeft.mFraction;
			Vector3 hitPos = Vector3(hitPosition.GetX(), hitPosition.GetY(), hitPosition.GetZ());

			float distance = Vector3(pOwner->GetWorldPosition() - hitPos).Length();
			if (distance < targetDistance)
			{
				moreLeft = true;
				avoidanceForce = right * avoidanceWeight * (8.0f - distance) * 1.5f;
				//OutputDebugStringA(("\nMore Left Hit into:" + no->GetName()).c_str());
			}
		}
		else if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayMoreRight, resultMoreRight, 
			IgnoreMultipleBroadPhaseLayerFilter({ BroadPhaseLayers::ENEMY, BroadPhaseLayers::TRIGGER, BroadPhaseLayers::PLAYER }),
			IgnoreMultipleObjectLayerFilter({ Layers::ENEMY, Layers::TRIGGER, Layers::PLAYER })))
		{
			Vec3 hitPosition = rayMoreRight.mOrigin + rayMoreRight.mDirection * resultMoreRight.mFraction;
			Vector3 hitPos = Vector3(hitPosition.GetX(), hitPosition.GetY(), hitPosition.GetZ());

			float distance = Vector3(pOwner->GetWorldPosition() - hitPos).Length();
			if (distance < targetDistance)
			{
				moreRight = true;
				avoidanceForce = -right * avoidanceWeight * (8.0f - distance) * 1.5f;
				//OutputDebugStringA(("\nMore Right Hit into:" + no->GetName()).c_str());
			}
		}
	}

	return avoidanceForce;
}
bool Walking::VoidCheck() 
{
	bool flag = false;
	Vector3 temporaryDirection = targetPosition - pOwner->GetWorldPosition();
	temporaryDirection.Normalize();

	float radius = 1.0f;

	Vector3 pos = pOwner->GetWorldPosition();

	Vector3 centerOrigin = pos + temporaryDirection * voidCheckRange;

	RRayCast ray1 = RRayCast(
		RVec3(centerOrigin.x, centerOrigin.y, centerOrigin.z),
		RVec3(0.0f, -10.0f, 0.0f)
	);
	RayCastResult result1;

	if (!PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(ray1, result1, 
		IgnoreMultipleBroadPhaseLayerFilter({ BroadPhaseLayers::ENEMY, BroadPhaseLayers::PLAYER, BroadPhaseLayers::TRIGGER }),
		IgnoreMultipleObjectLayerFilter({ Layers::ENEMY, Layers::PLAYER, Layers::TRIGGER })))
	{
		flag = true;
	}

	Vec3 dir = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID());
	if (dir.Length() > 1.1f)
	{
		dir = dir.Normalized();
		centerOrigin = pos + Vector3(dir.GetX(), dir.GetY(), dir.GetZ()) * voidCheckRange;
		RRayCast ray2 = RRayCast(
			RVec3(centerOrigin.x, centerOrigin.y, centerOrigin.z),
			RVec3(0.0f, -10.0f, 0.0f)
		);
		RayCastResult result2;
		if (!PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(ray2, result2,
			IgnoreMultipleBroadPhaseLayerFilter({ BroadPhaseLayers::ENEMY, BroadPhaseLayers::PLAYER, BroadPhaseLayers::TRIGGER }),
			IgnoreMultipleObjectLayerFilter({ Layers::ENEMY, Layers::PLAYER, Layers::TRIGGER })))
		{
			flag = true;
		}
	}
	
	voidNear = flag;
	return flag;
}

bool Walking::Jump()
{
	if (!canJump) return false;
	if (jumpTimer > 0.0f) return false;
	Vector3 temporaryDirection = targetPosition - pOwner->GetWorldPosition();
	temporaryDirection.y = 0.0f;
	temporaryDirection.Normalize();

	Vector3 jumpDirection = temporaryDirection + Vector3(0.0f, 0.5f, 0.0f);
	jumpDirection.Normalize();

	float radius = 1.0f;

	Vector3 pos = pOwner->GetWorldPosition();

	Vector3 centerOrigin = pos + temporaryDirection * jumpRange;

	RRayCast rayLeft = RRayCast(
		RVec3(centerOrigin.x, centerOrigin.y, centerOrigin.z),
		RVec3(0.0f, -50.0f, 0.0f)
	);
	RayCastResult resultLeft;
	if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayLeft, resultLeft, SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::GROUND), SpecifiedObjectLayerFilter(Layers::GROUND)))
	{
		PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(rigidbody->GetBodyID(), Vec3(0.0f, 0.0f, 0.0f));
		PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(rigidbody->GetBodyID(), Vec3(jumpDirection.x, jumpDirection.y, jumpDirection.z) * jumpForce);
		jumpTimer = 3.0f;
		return true;
	}
	return false;
}

void Walking::AutoJump()
{
	if (grounded)
	{
		Vector3 enemyPos = pOwner->GetWorldPosition();
		Vec3 tempVel = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID());
		Vector3 currentVelocity = { tempVel.GetX(), tempVel.GetY(), tempVel.GetZ() };
		Vector3 pos = enemyPos + currentVelocity * radius;

		float angleRad = DirectX::XMConvertToRadians(60.0f);
		Vector3 rightDir = Vector3(
			currentVelocity.x * cosf(angleRad) - currentVelocity.z * sinf(angleRad),
			currentVelocity.y,
			currentVelocity.x * sinf(angleRad) + currentVelocity.z * cosf(angleRad)
		);
		// Left: -10 degrees
		Vector3 leftDir = Vector3(
			currentVelocity.x * cosf(-angleRad) - currentVelocity.z * sinf(-angleRad),
			currentVelocity.y,
			currentVelocity.x * sinf(-angleRad) + currentVelocity.z * cosf(-angleRad)
		);

		Vector3 leftPos = enemyPos + leftDir * radius;
		Vector3 rightPos = enemyPos + rightDir * radius;

		// Central raycast (original)
		RRayCast rayFront = RRayCast(
			RVec3(pos.x, pos.y, pos.z),
			Vec3(0.0f, -(height / 2 + 0.2f), 0.0f)
		);
		RayCastResult resultFront;

		RRayCast rayLeft = RRayCast(
			RVec3(leftPos.x, leftPos.y, leftPos.z),
			Vec3(0.0f, -(height / 2 + 0.2f), 0.0f)
		);
		RayCastResult resultLeft;

		RRayCast rayRight = RRayCast(
			RVec3(rightPos.x, rightPos.y, rightPos.z),
			Vec3(0.0f, -(height / 2 + 0.2f), 0.0f)
		);
		RayCastResult resultRight;

		if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayFront, resultFront,
			IgnoreMultipleBroadPhaseLayerFilter({ BroadPhaseLayers::PLAYER, BroadPhaseLayers::ENEMY, BroadPhaseLayers::TRIGGER }),
			IgnoreMultipleObjectLayerFilter({ Layers::PLAYER, Layers::ENEMY, Layers::TRIGGER })))
		{
			Vec3 hitPos = rayFront.mOrigin + rayFront.mDirection * resultFront.mFraction;
			Vec3 playerDownPos = Vec3(enemyPos.x, enemyPos.y - (height / 2), enemyPos.z);
			float diffrence = (hitPos.GetY() - playerDownPos.GetY());
			if (diffrence < 2.5f && diffrence > 0.5f)
			{
				PhysicsCommon::physicsSystem->GetBodyInterface().SetPosition(rigidbody->GetBodyID(), Vec3(enemyPos.x, enemyPos.y + diffrence, enemyPos.z), EActivation::Activate);
			}
			Vec3 dir = hitPos - playerDownPos;
			dir.SetY(0.0f);
			dir = dir.Normalized();
			//PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(rigidbody->GetBodyID(), dir * 10.0f);
		}
		else if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayLeft, resultLeft,
			IgnoreMultipleBroadPhaseLayerFilter({ BroadPhaseLayers::PLAYER, BroadPhaseLayers::ENEMY, BroadPhaseLayers::TRIGGER }),
			IgnoreMultipleObjectLayerFilter({ Layers::PLAYER, Layers::ENEMY, Layers::TRIGGER })))
		{
			Vec3 hitPos = rayLeft.mOrigin + rayLeft.mDirection * resultLeft.mFraction;
			Vec3 playerDownPos = Vec3(enemyPos.x, enemyPos.y - (height / 2), enemyPos.z);
			float diffrence = (hitPos.GetY() - playerDownPos.GetY());
			if (diffrence < 2.5f && diffrence > 0.5f)
			{
				PhysicsCommon::physicsSystem->GetBodyInterface().SetPosition(rigidbody->GetBodyID(), Vec3(enemyPos.x, enemyPos.y + diffrence, enemyPos.z), EActivation::Activate);
			}
			Vec3 dir = hitPos - playerDownPos;
			dir.SetY(0.0f);
			dir = dir.Normalized();
			//PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(rigidbody->GetBodyID(), dir * 10.0f);
		}


		else if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayRight, resultRight,
			IgnoreMultipleBroadPhaseLayerFilter({ BroadPhaseLayers::PLAYER, BroadPhaseLayers::ENEMY, BroadPhaseLayers::TRIGGER }),
			IgnoreMultipleObjectLayerFilter({ Layers::PLAYER, Layers::ENEMY, Layers::TRIGGER })))
		{
			Vec3 hitPos = rayRight.mOrigin + rayRight.mDirection * resultRight.mFraction;
			Vec3 playerDownPos = Vec3(enemyPos.x, enemyPos.y - (height / 2), enemyPos.z);
			float diffrence = (hitPos.GetY() - playerDownPos.GetY());
			if (diffrence < 2.5f && diffrence > 0.5f)
			{
				PhysicsCommon::physicsSystem->GetBodyInterface().SetPosition(rigidbody->GetBodyID(), Vec3(enemyPos.x, enemyPos.y + diffrence, enemyPos.z), EActivation::Activate);
			}
			Vec3 dir = hitPos - playerDownPos;
			dir.SetY(0.0f);
			dir = dir.Normalized();
			//PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(rigidbody->GetBodyID(), dir * 10.0f);
		}



	}
}

void Walking::DrawImGuiControls()
{
	Vec3 velocity = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID());
	float vel = velocity.Length();
	ImGui::Text("Tag: %s", tag.c_str());
	ImGui::InputFloat("Max Speed", &maxSpeed);
	ImGui::InputFloat("Avoidance Weight", &avoidanceWeight);
	ImGui::InputFloat("Avoidance Distance", &avoidanceDistance);
	ImGui::InputFloat("Lerp", &rotationLerpFactor);
	ImGui::InputFloat("Jump Range", &jumpRange);
	ImGui::InputFloat("Jump Force", &jumpForce);
	ImGui::Checkbox("Grounded", &grounded);
	ImGui::Checkbox("Lefy Hit", &leftHit);
	ImGui::Checkbox("Right Hit", &rightHit);
	ImGui::Checkbox("More Left", &moreLeft);
	ImGui::Checkbox("More Right", &moreRight);
	ImGui::InputFloat("Velocity", &vel);
	ImGui::Checkbox("Void", &voidNear);
	ImGui::InputFloat("Time", &timerForChangedDirection);


}