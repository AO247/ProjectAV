#pragma once
#include "Drawable.h" 
#include <DirectXMath.h>
#include <SimpleMath.h>

class Graphics;

class SolidCapsule : public Drawable
{
public:
    SolidCapsule() = default;

    SolidCapsule(Graphics& gfx, DirectX::SimpleMath::Vector3 base, DirectX::SimpleMath::Vector3 tip, float radius);

    void SetBase(DirectX::SimpleMath::Vector3 base) noexcept;
    void SetTip(DirectX::SimpleMath::Vector3 tip) noexcept;
    void SetRadius(float radius) noexcept;

    DirectX::XMMATRIX GetTransformXM() const noexcept override;

    void Update(Graphics& gfx);
private:

    DirectX::SimpleMath::Vector3 basePos; 
    DirectX::SimpleMath::Vector3 tipPos;  
    float capRadius;                      
};