#pragma once
#include "Drawable.h" 
#include <DirectXMath.h>
#include <SimpleMath.h> // Include SimpleMath for Vector3

class Graphics; // Forward declare

class SolidCapsule : public Drawable
{
public:
    SolidCapsule() = default;

    // Constructor takes base, tip, radius for geometry creation
    SolidCapsule(Graphics& gfx, DirectX::SimpleMath::Vector3 base, DirectX::SimpleMath::Vector3 tip, float radius);

    // Setters for updating parameters for temporary drawing approach
    void SetBase(DirectX::SimpleMath::Vector3 base) noexcept;
    void SetTip(DirectX::SimpleMath::Vector3 tip) noexcept;
    void SetRadius(float radius) noexcept;

    // GetTransformXM will likely just return identity or a simple translation
    // if geometry is created in world space each frame.
    DirectX::XMMATRIX GetTransformXM() const noexcept override;

    // --- Move/Copy semantics ---
    SolidCapsule(SolidCapsule&&) noexcept = default;
    SolidCapsule& operator=(SolidCapsule&&) noexcept = default;
    SolidCapsule(const SolidCapsule&) = delete;
    SolidCapsule& operator=(const SolidCapsule&) = delete;

private:
    // Store parameters needed for GetTransformXM (if any) 
    // or potentially for rebuilding geometry if Setters modify size/radius.
    DirectX::SimpleMath::Vector3 basePos = { 0,0,0 }; // Store world base for translation
    DirectX::SimpleMath::Vector3 tipPos = { 0,1,0 };  // Store world tip
    float capRadius = 0.5f;                           // Store radius
};