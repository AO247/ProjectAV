#include "Walking.h"
#include "Node.h"       
#include "Window.h" 
#include "CMath.h"
#include <DirectXMath.h>
#include <algorithm>

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

    Vector3 targetPos(targetPosition.x, targetPosition.y, targetPosition.z);
    Vector3 currentPos = rigidbody->GetPosition(); 
    Vector3 currentVelocity = rigidbody->GetVelocity();

    Vector3 desiredDirection = targetPos - currentPos;

    desiredDirection.Normalize();
    Vector3 desiredVelocity = desiredDirection * maxSpeed;

    Vector3 steeringForce = desiredVelocity - currentVelocity;

    float steeringMagnitude = steeringForce.Length();
    if (steeringMagnitude > maxForce) {
        steeringForce = (steeringForce / steeringMagnitude) * maxForce; 
    }

    rigidbody->AddForce(steeringForce * 10.0f);


    if (currentVelocity.LengthSquared() > 0.01f) 
    {
        Vector3 facingDirection = currentVelocity; 
        facingDirection.Normalize();

        float targetYaw = atan2f(facingDirection.x, facingDirection.z);

        GetOwner()->SetLocalRotation({ 0.0f, targetYaw, 0.0f });
    }
}



void Walking::DrawImGuiControls()
{
   ImGui::Text("Player Controller Properties:");
   ImGui::Text("Tag: %s", tag.c_str());
   ImGui::InputFloat("Max Speed", &maxSpeed);
   ImGui::InputFloat("Max Force", &maxForce);

   //ImGui::InputFloat("JumpForce", &jumpForce);
   //ImGui::Checkbox("Jumped", &jumped); // Display jump status
}