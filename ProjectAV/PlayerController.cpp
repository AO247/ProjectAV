#include "PlayerController.h"
#include "Node.h"       // Include Node to call SetLocalPosition/Rotation
#include "Window.h"     // Included via header, but good practice
#include "CMath.h"      // For wrap_angle and PI (ensure this is included)
#include <DirectXMath.h>
#include <algorithm> // for std::clamp

namespace dx = DirectX;
PlayerController::PlayerController(Node* owner, Window& window)
    : Component(owner), wnd(window) // Initialize reference member
{
	rigidbody = owner->GetComponent<Rigidbody>();
	camera = owner->GetRoot()->FindFirstChildByTag("Camera");
}

void PlayerController::Update(float dt)
{
	camera->SetLocalPosition({ GetOwner()->GetLocalPosition().x, GetOwner()->GetLocalPosition().y, GetOwner()->GetLocalPosition().z });
	GetOwner()->SetLocalRotation({ 0.0f, camera->GetLocalRotationEuler().y, 0.0f});

    if (!wnd.CursorEnabled())
    {
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
    if (!jumped) {
        rigidbody->SetVelocity(Vector3(rigidbody->GetVelocity().x, 0.0f, rigidbody->GetVelocity().z));
        rigidbody->AddForce(Vector3(0.0f, jumpForce * 1000.0f, 0.0f));
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

void PlayerController::KeyboardInput()
{
	moveDirection = Vector3(0.0f, 0.0f, 0.0f);

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

void PlayerController::MovePlayer()
{
    moveDirection.Normalize();

    rigidbody->AddForce(moveDirection * moveSpeed * 1000.0f);
}

void PlayerController::DrawImGuiControls()
{
    ImGui::Text("Player Controller Properties:");
    ImGui::InputFloat("Move Speed", &moveSpeed);
    ImGui::InputFloat("JumpForce", &jumpForce);
	ImGui::InputFloat("Dash Force", &dashForce);
    ImGui::Checkbox("Jumped", &jumped);
	ImGui::Checkbox("Dashed", &dashed);
}