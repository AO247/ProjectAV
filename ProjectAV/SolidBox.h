#pragma once
#include "Drawable.h" 
#include <DirectXMath.h>

class SolidBox : public Drawable
{
public:
    SolidBox() = default;

    SolidBox(Graphics& gfx, DirectX::XMFLOAT3 center, DirectX::XMFLOAT3 size);

    void SetPos(DirectX::XMFLOAT3 center) noexcept;
	void SetSize(DirectX::XMFLOAT3 size) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	DirectX::XMFLOAT3 center = { 0.0f, 0.0f, 0.0f }; // Center of the box
	DirectX::XMFLOAT3 size = { 1.0f, 1.0f, 1.0f }; // Size of the box
};

