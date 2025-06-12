#include "Camera.h" 
#include "Node.h"          
#include "Window.h"        
#include "CMath.h"         
#include <DirectXMath.h>
#include <algorithm>       

namespace dx = DirectX;

Camera::Camera(Node* owner, Window& window)
    : Component(owner),
    wnd(window)
{

}

void Camera::Update(float dt)
{
    if (!wnd.CursorEnabled() && GetOwner() && active && !wnd.playerLocked)
    {
        HandleMouseLook(dt);
    }
}

void Camera::HandleMouseLook(float dt)
{
    float mouseX = 0.0f;
    float mouseY = 0.0f;

    while (const auto delta = wnd.mouse.ReadRawDelta())
    {
        mouseX += delta->x;
        mouseY -= delta->y;
    }

    yRotation += mouseX * sensX * 0.01f; 
    xRotation -= mouseY * sensY * 0.01f; 

    xRotation = std::clamp(xRotation, -pitchLimit, pitchLimit);
    yRotation = wrap_angle(yRotation);
    DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationRollPitchYaw(xRotation, yRotation, 0.0f);
    DirectX::XMFLOAT4 quatFloat4;
    DirectX::XMStoreFloat4(&quatFloat4, quat);

    GetOwner()->SetLocalRotation(quatFloat4);      
}


DirectX::XMMATRIX Camera::GetViewMatrix() const noexcept
{
    dx::XMMATRIX cameraWorldTransform = GetOwner()->GetWorldTransform();

    dx::XMVECTOR determinant = dx::XMMatrixDeterminant(cameraWorldTransform);
    return dx::XMMatrixInverse(&determinant, cameraWorldTransform);
}