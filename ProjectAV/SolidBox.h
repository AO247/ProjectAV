#pragma once
#include "Drawable.h" // Assuming Drawable base class definition
#include <DirectXMath.h>

class SolidBox : public Drawable
{
public:
    // Default constructor: Leaves the object uninitialized
    SolidBox() = default;

    // Constructor that initializes directly
    SolidBox(Graphics& gfx, DirectX::XMFLOAT3 minExtents, DirectX::XMFLOAT3 maxExtents);

    // Initialization function (allows default construction then initialization)
    void Initialize(Graphics& gfx, DirectX::XMFLOAT3 minExtents, DirectX::XMFLOAT3 maxExtents);

    // Set the position (translation) of the AABB in the world
    void SetPos(DirectX::XMFLOAT3 pos) noexcept;

    // Get the transformation matrix (currently only translation)
    DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
    DirectX::XMFLOAT3 pos = { 0.0f, 0.0f, 0.0f }; // Default position at origin
};