#pragma once
#include "Drawable.h" 
#include <DirectXMath.h>

class Graphics; // Forward declare

class SolidBox : public Drawable
{
public:
    SolidBox() = default;

    // **** CHANGED: Constructor takes center and size ****
    SolidBox(Graphics& gfx, DirectX::XMFLOAT3 center, DirectX::XMFLOAT3 size);

    // **** CHANGED: Initialize takes center and size ****
    void Initialize(Graphics& gfx, DirectX::XMFLOAT3 center, DirectX::XMFLOAT3 size);

    // --- Move/Copy semantics (as before) ---
    SolidBox(SolidBox&&) noexcept = default;
    SolidBox& operator=(SolidBox&&) noexcept = default;
    SolidBox(const SolidBox&) = delete;
    SolidBox& operator=(const SolidBox&) = delete;

    // --- SetPos is no longer needed if using SetTransformXM ---
    // void SetPos(DirectX::XMFLOAT3 pos) noexcept; 

    DirectX::XMMATRIX GetTransformXM() const noexcept override;
    void SetTransformXM(DirectX::FXMMATRIX matrix) noexcept; // Use FXMMATRIX for input consistency

private:
    // Position is now part of the matrix, store only the matrix
    DirectX::XMFLOAT4X4 matrix = {}; // Store as XMFLOAT4X4
};