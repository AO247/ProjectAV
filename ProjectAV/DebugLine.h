#pragma once
#include "Drawable.h"
#include <DirectXMath.h>
#include "Vertex.h"

class DebugLine : public Drawable
{
public:
    DebugLine() = default;

    DebugLine(Graphics& gfx, DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f });


    DirectX::XMMATRIX GetTransformXM() const noexcept override;


    void SetPoints(Graphics& gfx, DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end);

private:

    DirectX::XMFLOAT3 startPoint = { 0,0,0 };
    DirectX::XMFLOAT3 endPoint = { 0,0,1 };


    std::string GenerateLineTag(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end);
};