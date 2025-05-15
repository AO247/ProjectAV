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
void Walking::Follow(DirectX::XMFLOAT3 targetPos)
{
    /*if (!rigidbody || rigidbody->GetIsStatic()) { 
        return;
    }
	targetPosition = targetPos;
    Vector3 currentPos = rigidbody->GetPosition(); 
    Vector3 currentVelocity = rigidbody->GetVelocity();*/

    //Vector3 desiredDirection = targetPosition - currentPos;

    /*desiredDirection.Normalize();
    Vector3 desiredVelocity = desiredDirection * maxSpeed;

    Vector3 steeringForce = desiredVelocity - currentVelocity;*/

	//steeringForce = steeringForce + (CalculateAvoidanceForce());

    //float steeringMagnitude = steeringForce.Length();
    /*if (steeringMagnitude > maxForce) {
        steeringForce = (steeringForce / steeringMagnitude) * maxForce; 
    }*/

	/*rigidbody->AddForce(steeringForce * 10.0f);

	currentVelocity = rigidbody->GetVelocity();*/
    /*if (currentVelocity.LengthSquared() > 0.01f) 
    {
        Vector3 facingDirection = currentVelocity; 
        facingDirection.Normalize();


		float currentYaw = pOwner->GetLocalRotationEuler().y;
		float targetYaw = atan2f(facingDirection.x, facingDirection.z);

		float yawDifference = targetYaw - currentYaw;

		targetYaw = currentYaw + yawDifference;

        pOwner->SetLocalRotation({ 0.0f, targetYaw, 0.0f });
    }

	if (pOwner->GetComponent<SoundEffectsPlayer>()->isPlaying() == false) {
		pOwner->GetComponent<SoundEffectsPlayer>()->Play(0);
	}*/
}

Vector3 Walking::CalculateAvoidanceForce()
{
	Vector3 avoidanceForce(0.0f, 0.0f, 0.0f);

	Vector3 previousRotation = pOwner->GetLocalRotationEuler();
	pOwner->TranslateLocal({ 0.0f, 1.0f, 0.0f });
	Vector3 temporaryDirection = targetPosition - pOwner->GetWorldPosition();
	temporaryDirection.Normalize();
	float targetYaw = atan2f(temporaryDirection.x, temporaryDirection.z);
	pOwner->SetLocalRotation({ 0.0f, targetYaw, 0.0f });

	//float radius = pOwner->GetComponent<CapsuleCollider>()->GetRadius();

	//Vector3 pos = pOwner->GetComponent<CapsuleCollider>()->GetTransformedBase();
	Vector3 forward = pOwner->Forward();
	Vector3 right = pOwner->Right();

	leftHit = false;
	rightHit = false;

	//Vector3 centerOrigin = pos + forward;
	/*Vector3 leftOrigin = centerOrigin - right * radius;
	Vector3 rightOrigin = centerOrigin + right * radius;*/

	Vector3 centerDir = forward;
	
	Vector3 leftDir = (forward - right * 0.5f); leftDir.Normalize(); 

	Vector3 rightDir = (forward + right * 0.5f); rightDir.Normalize();

	/*RaycastData hitLeft = Raycast::CastThroughLayers(leftOrigin, centerDir, std::vector<Layers>{ENEMY, PLAYER});


	RaycastData hitRight = Raycast::CastThroughLayers(rightOrigin, centerDir, std::vector<Layers>{ENEMY, PLAYER});

	float targetDistance = Vector3(pos - targetPosition).Length();
	float distance = Vector3(pos - hitLeft.hitPoint).Length();
	if (hitLeft.hitCollider != nullptr && Vector3(hitLeft.hitPoint - pos).Length() < targetDistance && 
		Vector3(hitLeft.hitPoint - pos).Length() < avoidanceDistance) {
		leftHit = true;
		
		avoidanceForce = right * avoidanceWeight;
	}

	if (hitRight.hitCollider != nullptr && Vector3(hitRight.hitPoint - pos).Length() < targetDistance && 
		Vector3(hitRight.hitPoint - pos).Length() < avoidanceDistance) {
		rightHit = true;
		avoidanceForce = -right * avoidanceWeight;
	}

	if (leftHit && rightHit) {
		RaycastData moreLeft = Raycast::CastThroughLayers(leftOrigin, leftDir, std::vector<Layers>{ENEMY, PLAYER});
		RaycastData moreRight = Raycast::CastThroughLayers(rightOrigin, rightDir, std::vector<Layers>{ENEMY, PLAYER});

		if (moreLeft.hitCollider != nullptr && Vector3(moreLeft.hitPoint - pos).Length() < targetDistance && 
			Vector3(moreLeft.hitPoint - pos).Length() < avoidanceDistance) {
			avoidanceForce = right * avoidanceWeight;
		}
		else if (moreRight.hitCollider != nullptr && Vector3(moreRight.hitPoint - pos).Length() < targetDistance && 
			Vector3(moreRight.hitPoint - pos).Length() < avoidanceDistance) {
			avoidanceForce = -right * avoidanceWeight;
		}
	}*/

	pOwner->SetLocalRotation(previousRotation);
	pOwner->TranslateLocal({ 0.0f, -1.0f, 0.0f });

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