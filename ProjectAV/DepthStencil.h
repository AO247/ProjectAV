#pragma once
#include "Graphics.h"
#include "GraphicsResource.h"

class DepthStencil : public GraphicsResource
{
    friend class RenderTarget;
    friend class Graphics; // If Graphics directly uses pDepthStencilView
public:
    DepthStencil(Graphics& gfx, UINT width, UINT height);
    void BindAsDepthStencil(Graphics& gfx) const noexcept;
    void Clear(Graphics& gfx) const noexcept;
    void BindAsTexture(Graphics& gfx, UINT slot, bool forPixelShader = true) const noexcept; // Added method, slot for PS or VS

private:
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView; // Member for SRV
};