#pragma once
#include "Drawable.h"
#include <DirectXMath.h>

class Graphics;

class ColliderSphere : public Drawable
{
public:
    ColliderSphere() = default;

    ColliderSphere(Graphics& gfx, float radius);

    void SetPos(DirectX::XMFLOAT3 pos) noexcept;

    DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
    DirectX::XMFLOAT3 pos = { 0.0f, 0.0f, 0.0f };
};