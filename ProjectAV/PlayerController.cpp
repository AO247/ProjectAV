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
    dx::XMVECTOR translation = dx::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    bool moved = false;

    // Calculate translation vector based on WASD keys
    if (wnd.kbd.KeyIsPressed('W'))
    {
        translation = dx::XMVectorAdd(translation, dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
        //rigidbody->AddForce(Vector3(0.0f, 0.0f, 200.0f));
        moved = true;
    }
    if (wnd.kbd.KeyIsPressed('S'))
    {
        translation = dx::XMVectorAdd(translation, dx::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f));
        //rigidbody->AddForce(Vector3(translation));
        moved = true;
    }
    if (wnd.kbd.KeyIsPressed('A'))
    {
        translation = dx::XMVectorAdd(translation, dx::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f));
        //rigidbody->AddForce(Vector3(translation));
        moved = true;
    }
    if (wnd.kbd.KeyIsPressed('D'))
    {
        translation = dx::XMVectorAdd(translation, dx::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f));
        //rigidbody->AddForce(Vector3(translation));
        moved = true;
    }
    if (wnd.kbd.KeyIsPressed(VK_SHIFT))
    {
		moveSpeed = 100000.0f;
	}
	else
	{
        moveSpeed = 30000.0f;
	}
    // Optional: Add Up/Down movement
    if (wnd.kbd.KeyIsPressed(VK_SPACE))
    {
        translation = dx::XMVectorAdd(translation, dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
        moved = true;
    }
    if (wnd.kbd.KeyIsPressed(VK_CONTROL))
    {
        translation = dx::XMVectorAdd(translation, dx::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));
        moved = true;
    }


    if (moved)
    {
        translation = dx::XMVector3Normalize(translation);
        translation = dx::XMVectorScale(translation, moveSpeed * dt);

        dx::XMMATRIX rotationMatrix = dx::XMMatrixRotationRollPitchYaw(0.0f, yaw, 0.0f);
        translation = dx::XMVector3Transform(translation, rotationMatrix);

        DirectX::XMFLOAT3 currentPos = GetOwner()->GetLocalPosition();
        dx::XMVECTOR newPosVec = dx::XMLoadFloat3(&currentPos);
        newPosVec = dx::XMVectorAdd(newPosVec, translation);


        DirectX::XMFLOAT3 newPos;
        dx::XMStoreFloat3(&newPos, newPosVec);
        //GetOwner()->SetLocalPosition(newPos);
        rigidbody->AddForce(translation);
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