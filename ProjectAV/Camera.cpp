#include "Camera.h" // Include the header for this class
#include "Node.h"            // Include Node for GetOwner/SetLocalRotation
#include "Window.h"          // For wnd access
#include "CMath.h"           // For wrap_angle, PI
#include <DirectXMath.h>
#include <algorithm>        // For std::clamp

namespace dx = DirectX;

Camera::Camera(Node* owner, Window& window)
    : Component(owner),
    wnd(window)
{

}

void Camera::Update(float dt)
{
    // Don't process look if cursor is enabled OR if required nodes are missing
    if (!wnd.CursorEnabled() && GetOwner() && active)
    {
        HandleMouseLook(dt);
    }
}

void Camera::HandleMouseLook(float dt)
{
    float mouseX = 0.0f;
    float mouseY = 0.0f;

    // Accumulate mouse delta for this frame
    while (const auto delta = wnd.mouse.ReadRawDelta())
    {
        mouseX += delta->x;
        mouseY -= delta->y;
    }

    yRotation += mouseX * sensX * 0.01f; // Apply Yaw delta (adjust multiplier)
    xRotation -= mouseY * sensY * 0.01f; // Apply Pitch delta (inverted Y, adjust multiplier)

    // Clamp pitch
    xRotation = std::clamp(xRotation, -pitchLimit, pitchLimit);
    // Wrap Yaw (optional, but good practice)
    yRotation = wrap_angle(yRotation);

    GetOwner()->SetLocalRotation({xRotation, yRotation, 0.0f});          // Apply Pitch to camera node's X axis
}


DirectX::XMMATRIX Camera::GetViewMatrix() const noexcept
{
    dx::XMMATRIX cameraWorldTransform = GetOwner()->GetWorldTransform();

    dx::XMVECTOR determinant = dx::XMMatrixDeterminant(cameraWorldTransform);
    return dx::XMMatrixInverse(&determinant, cameraWorldTransform);
}