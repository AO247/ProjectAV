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
}

void PlayerController::Update(float dt)
{
    if (!wnd.CursorEnabled())
    {
        HandleMouseLookInput();
        HandleMovementInput(dt); // Pass delta time for movement calculation
    }
}

void PlayerController::HandleMouseLookInput()
{
    while (const auto delta = wnd.mouse.ReadRawDelta())
    {
        yaw = wrap_angle(yaw + delta->x * rotationSpeed);
        pitch = std::clamp(pitch + delta->y * rotationSpeed, -pitchLimit, pitchLimit);
    }

    GetOwner()->SetLocalRotation({ 0.0f, yaw, 0.0f }); // Set Node's Y rotation (Yaw) ONLY
}

void PlayerController::HandleMovementInput(float dt)
{
	Vector3 moveDirection = Vector3(0.0f, 0.0f, 0.0f);
    bool moved = false;

    // Calculate translation vector based on WASD keys
    if (wnd.kbd.KeyIsPressed('W'))
    {
		moveDirection += Vector3(0.0f, 0.0f, 1.0f);
        //rigidbody->AddForce(Vector3(0.0f, 0.0f, 200.0f));
        moved = true;

    }
    if (wnd.kbd.KeyIsPressed('S'))
    {
		moveDirection += Vector3(0.0f, 0.0f, -1.0f);
        //rigidbody->AddForce(Vector3(translation));
        moved = true;
    }
    if (wnd.kbd.KeyIsPressed('A'))
    {
		moveDirection += Vector3(-1.0f, 0.0f, 0.0f);
        //rigidbody->AddForce(Vector3(translation));
        moved = true;
    }
    if (wnd.kbd.KeyIsPressed('D'))
    {
		moveDirection += Vector3(1.0f, 0.0f, 0.0f);
        //rigidbody->AddForce(Vector3(translation));
        moved = true;
    }
    if (wnd.kbd.KeyIsPressed(VK_SHIFT))
    {
		moveSpeed = 30.0f;
	}
	else
	{
        moveSpeed = 12.0f;
	}
    // Optional: Add Up/Down movement
    if (wnd.kbd.KeyIsPressed(VK_SPACE))
    {
        if (!jumped) {
			rigidbody->SetVelocity(Vector3(rigidbody->GetVelocity().x, 0.0f, rigidbody->GetVelocity().z));
			rigidbody->AddForce(Vector3(0.0f, jumpForce * 1000.0f, 0.0f));
			jumped = true;
        }
        moved = true;
    }
    else {
		jumped = false;
    }
    if (wnd.kbd.KeyIsPressed(VK_CONTROL))
    {
        moveDirection += Vector3(0.0f, -1.0f, 0.0f);
        moved = true;
    }

    

    if (moved)
    {
        moveDirection.Normalize();


       /* dx::XMMATRIX rotationMatrix = dx::XMMatrixRotationRollPitchYaw(0.0f, yaw, 0.0f);
		Matrix rotation = Matrix(rotationMatrix);
		moveDirection.Transform(moveDirection, rotation);*/

        dx::XMMATRIX rotationMatrix = dx::XMMatrixRotationRollPitchYaw(0.0f, yaw, 0.0f);
        dx::XMVECTOR moveVector = dx::XMLoadFloat3(&moveDirection);
        moveVector = dx::XMVector3Transform(moveVector, rotationMatrix);
        dx::XMStoreFloat3(&moveDirection, moveVector);

        rigidbody->AddForce(moveDirection * moveSpeed * 100.0f);
    }
}

DirectX::XMFLOAT2 PlayerController::GetRotation() const
{
    return { pitch, yaw };
}

void PlayerController::SetRotation(float pitchRad, float yawRad) noexcept
{
    pitch = std::clamp(pitchRad, -pitchLimit, pitchLimit);
    yaw = wrap_angle(yawRad);
    // Immediately update the node's horizontal rotation
    GetOwner()->SetLocalRotation({ 0.0f, yaw, 0.0f });
}

void PlayerController::DrawImGuiControls()
{
    ImGui::Text("Player Controller Properties:");
    ImGui::InputFloat("Move Speed", &moveSpeed);
    ImGui::InputFloat("JumpForce", &jumpForce);
    ImGui::Checkbox("Jumped", &jumped); // Display jump status
}