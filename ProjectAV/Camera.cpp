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
    if (!wnd.CursorEnabled() && pOwner != nullptr && active && !wnd.playerLocked)
    {
        HandleMouseLook(dt);
    }
}

void Camera::HandleMouseLook(float dt)
{
    float mouseX = 0.0f;
    float mouseY = 0.0f;

   
    while (const auto delta = wnd.mouse.PollRawMovement())
    {
        mouseX += delta->dx;
        mouseY -= delta->dy;
    }

    yRotation += mouseX * sensX * 0.01f; 
    xRotation -= mouseY * sensY * 0.01f; 

    xRotation = std::clamp(xRotation, -pitchLimit, pitchLimit);
    yRotation = wrap_angle(yRotation);
    DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationRollPitchYaw(xRotation, yRotation, 0.0f);
    DirectX::XMFLOAT4 quatFloat4;
    DirectX::XMStoreFloat4(&quatFloat4, quat);

    pOwner->SetLocalRotation(quatFloat4);
}


DirectX::XMMATRIX Camera::GetViewMatrix() const noexcept
{
    dx::XMMATRIX cameraWorldTransform = pOwner->GetWorldTransform();

    dx::XMVECTOR determinant = dx::XMMatrixDeterminant(cameraWorldTransform);
    return dx::XMMatrixInverse(&determinant, cameraWorldTransform);
}

DirectX::XMFLOAT3 Camera::GetPos() const noexcept
{
    // Pobierz macierz transformacji œwiata wêz³a, którego w³aœcicielem jest kamera
    dx::XMMATRIX cameraWorldTransform = GetOwner()->GetWorldTransform();

    // Pozycja kamery to ostatnia kolumna (wektor translacji) tej macierzy
    // (zak³adaj¹c, ¿e macierz jest w formacie kolumnowym, co jest typowe dla DirectXMath)
    // Macierze XMMATRIX s¹ rzêdowo-g³ówne (row-major) w pamiêci, ale konwencje DirectX
    // czêsto traktuj¹ je jako kolumnowo-g³ówne w operacjach.
    // Kolumna translacji to _41, _42, _43.
    // Mo¿na te¿ u¿yæ dekompozycji.
    // Najproœciej:
    DirectX::XMFLOAT3 position;
    position.x = cameraWorldTransform.r[3].m128_f32[0]; // lub _41 jeœli masz dostêp do pól struktury
    position.y = cameraWorldTransform.r[3].m128_f32[1]; // lub _42
    position.z = cameraWorldTransform.r[3].m128_f32[2]; // lub _43
    return position;

    // Alternatywnie, jeœli Node::GetWorldTransform() zwraca macierz, która mo¿e mieæ skalowanie,
    // a chcesz "czyst¹" pozycjê bez skalowania propagowanego z rodziców,
    // i jeœli Node ma metodê GetWorldPosition():
    // return GetOwner()->GetWorldPosition();
    // W przeciwnym razie, powy¿szy sposób jest poprawny dla standardowej macierzy transformacji.
}