#include "PlayerController.h"
#include "Node.h"       // Include Node to call SetLocalPosition/Rotation
#include "Window.h"     // Included via header, but good practice
#include "CMath.h"      // For wrap_angle and PI (ensure this is included)
#include <DirectXMath.h>
#include <algorithm> // for std::clamp
#include "Raycast.h"
#include <string>
#include "CapsuleCollider.h"
#include "BoundingSphere.h"
namespace dx = DirectX;
PlayerController::PlayerController(Node* owner, Window& window)
    : Component(owner), wnd(window) // Initialize reference member
{
	rigidbody = owner->GetComponent<Rigidbody>();
	camera = owner->GetRoot()->FindFirstChildByTag("Camera");
	ability1 = owner->GetRoot()->FindFirstChildByTag("Ability1");
	ability2 = owner->GetRoot()->FindFirstChildByTag("Ability2");
}

void PlayerController::Update(float dt)
{
	camera->SetLocalPosition({ GetOwner()->GetLocalPosition().x, GetOwner()->GetLocalPosition().y + height * 9 /10, GetOwner()->GetLocalPosition().z });
    GetOwner()->SetLocalRotation({ 0.0f, camera->GetLocalRotationEuler().y, 0.0f });
	ability1->SetLocalPosition(camera->GetLocalPosition());
	ability1->SetLocalRotation(camera->GetLocalRotationEuler());

    RaycastData rayData = Raycast::Cast(camera->GetWorldPosition(), camera->Forward());

    if (rayData.hitCollider != nullptr)
    {
        if (rayData.hitCollider->GetOwner()->tag == "Ground" || rayData.hitCollider->GetOwner()->tag == "Stone")
        {
            ability2->SetLocalPosition(rayData.hitPoint);
        }
    }



    if (!wnd.CursorEnabled())
    {
        GroundCheck();
		KeyboardInput();
		SpeedControl();
		MovePlayer();
    }
}


void PlayerController::SpeedControl()
{
    Vector3 velocity(rigidbody->GetVelocity().x, 0.0f, rigidbody->GetVelocity().z);

    if (velocity.Length() > moveSpeed) 
    {
		velocity.Normalize();
        Vector3 limitedVel = velocity * moveSpeed;
		rigidbody->SetVelocity(Vector3(limitedVel.x, rigidbody->GetVelocity().y, limitedVel.z));
    }
	if (moveDirection.Length() == 0)
	{
		Vector3 currentVelocity = rigidbody->GetVelocity();
		currentVelocity.x = currentVelocity.x / 1.3f;
		currentVelocity.z = currentVelocity.z / 1.3f;
		rigidbody->SetVelocity(currentVelocity);
	}
}

void PlayerController::Jump()
{
    if (grounded && !jumped) {
        rigidbody->SetVelocity(Vector3(rigidbody->GetVelocity().x, 0.0f, rigidbody->GetVelocity().z));
        rigidbody->AddForce(Vector3(0.0f, jumpForce * 1000.0f, 0.0f));
        grounded = false;
		jumped = true;
    }
}

void PlayerController::Dash()
{
    if (!dashed) {
        Vector3 dashDirection = moveDirection;
        dashDirection.Normalize();
        rigidbody->AddForce(dashDirection * dashForce * 1000.0f);
        dashed = true;
    }
}

void PlayerController::GroundCheck()
{
	RaycastData rayData = Raycast::Cast(GetOwner()->GetWorldPosition(), Vector3(0.0f, -1.0f, 0.0f));
	if (rayData.hitCollider != nullptr)
	{
		if (rayData.hitCollider->GetOwner()->tag == "Ground")
		{

            if (GetOwner()->GetWorldPosition().y - rayData.hitPoint.y <= 0.01)
			{
				grounded = true;
			}
			else
			{
				grounded = false;
			}
		}

	}
}

void PlayerController::MovePlayer()
{
    moveDirection.Normalize();

    rigidbody->AddForce(moveDirection * moveSpeed * 1000.0f);
}

void PlayerController::Ability1()
{
    CapsuleCollider* col = ability1->GetComponent<CapsuleCollider>();
    std::vector<Collider*> colliders = col->GetTriggerStay();
	for (Collider* collider : colliders)
	{
		if (collider->GetOwner()->tag == "Enemy" || collider->GetOwner()->tag == "Stone")
		{
			collider->GetOwner()->GetComponent<Rigidbody>()->SetVelocity(Vector3(0.0f, 0.0f, 0.0f));
			collider->GetOwner()->GetComponent<Rigidbody>()->AddForce(ability1->Forward() * 100000.0f);
		}
	}
}

void PlayerController::Ability2()
{
    
	BoundingSphere* col = ability2->GetComponent<BoundingSphere>();

    std::vector<Collider*> colliders = col->GetTriggerStay();

    for (Collider* collider : colliders)
    {
        if (collider->GetOwner()->tag == "Enemy" || collider->GetOwner()->tag == "Stone")
        {
            collider->GetOwner()->GetComponent<Rigidbody>()->SetVelocity(Vector3(0.0f, 0.0f, 0.0f));
            collider->GetOwner()->GetComponent<Rigidbody>()->AddForce(Vector3(0.0f, 70000.0f, 0.0f));
        }
    }


}


void PlayerController::KeyboardInput()
{
    moveDirection = Vector3(0.0f, 0.0f, 0.0f);

    if (wnd.mouse.LeftIsPressed())
    {
        Ability1();
    }
	if (wnd.mouse.RightIsPressed())
	{
		Ability2();
	}

    if (wnd.kbd.KeyIsPressed('W'))
    {
        moveDirection += GetOwner()->Forward();
    }
    if (wnd.kbd.KeyIsPressed('S'))
    {
        moveDirection += GetOwner()->Back();
    }
    if (wnd.kbd.KeyIsPressed('A'))
    {
        moveDirection += GetOwner()->Left();
    }
    if (wnd.kbd.KeyIsPressed('D'))
    {
        moveDirection += GetOwner()->Right();
    }
    if (wnd.kbd.KeyIsPressed(VK_SPACE))
    {
        Jump();
    }
    else {
        jumped = false;
    }
    if (wnd.kbd.KeyIsPressed(VK_SHIFT))
    {
        Dash();
    }
    else {
        dashed = false;
    }

}
void PlayerController::DrawImGuiControls()
{
    ImGui::Text("Player Controller Properties:");
    ImGui::InputFloat("Move Speed", &moveSpeed);
    ImGui::InputFloat("JumpForce", &jumpForce);
	ImGui::InputFloat("Dash Force", &dashForce);
    ImGui::Checkbox("Jumped", &jumped);
	ImGui::Checkbox("Dashed", &dashed);
	ImGui::Checkbox("Grounded", &grounded);
}