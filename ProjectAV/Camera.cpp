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
    if (!wnd.CursorEnabled() && GetOwner() && active && !wnd.playerLocked)
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
    DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationRollPitchYaw(xRotation, yRotation, 0.0f);
    DirectX::XMFLOAT4 quatFloat4;
    DirectX::XMStoreFloat4(&quatFloat4, quat);

    GetOwner()->SetLocalRotation(quatFloat4);      // Apply Pitch to camera node's X axis
}


DirectX::XMMATRIX Camera::GetViewMatrix() const noexcept
{
    dx::XMMATRIX cameraWorldTransform = GetOwner()->GetWorldTransform();

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