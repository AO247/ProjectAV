#pragma once
#include "Drawable.h"

class SolidBox : public Drawable
{
public:
	SolidBox(Graphics& gfx, DirectX::XMFLOAT3 minExtents, DirectX::XMFLOAT3 maxExtents);
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	DirectX::XMFLOAT3 pos = { 1.0f,1.0f,1.0f };
};

