#pragma once
#include "Drawable.h" // Your new Drawable base class
#include <DirectXMath.h>

class Graphics; // Forward declare

class ColliderSphere : public Drawable
{
public:
    // Default constructor: often useful for map storage if you revert from temp drawing
    ColliderSphere() = default;

    // Main constructor that sets up the drawable
    ColliderSphere(Graphics& gfx, float radius);

    // Set the position (used by GetTransformXM)
    void SetPos(DirectX::XMFLOAT3 pos) noexcept;

    // Required by Drawable
    DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
    // Stores the position for the translation matrix
    DirectX::XMFLOAT3 pos = { 0.0f, 0.0f, 0.0f }; // Default to origin
};