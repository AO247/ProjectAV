#pragma once
#include "Drawable.h"
#include <DirectXMath.h>
#include "Vertex.h" // For Dvtx::* types

class DebugLine : public Drawable
{
public:
    DebugLine() = default; // Needed for potential map storage later
    // Constructor takes Graphics, start/end points, and color
    DebugLine(Graphics& gfx, DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f });

    // No specific transform needed if vertices are in world space
    DirectX::XMMATRIX GetTransformXM() const noexcept override;

    // Update the line's points (optional, if reusing the object)
    void SetPoints(Graphics& gfx, DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end);

private:
    // Store the world-space points used to generate geometry
    DirectX::XMFLOAT3 startPoint = { 0,0,0 };
    DirectX::XMFLOAT3 endPoint = { 0,0,1 };

    // Need a unique identifier if caching geometry in Codex
    std::string GenerateLineTag(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end);
};