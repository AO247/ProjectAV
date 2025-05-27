#include "DepthStencil.h"
#include "GraphicsThrowMacros.h"

namespace wrl = Microsoft::WRL;

DepthStencil::DepthStencil(Graphics& gfx, UINT width, UINT height)
{
    INFOMAN(gfx);

    // create depth stencil texture
    wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1u;
    descDepth.ArraySize = 1u;
    // Format for the texture resource itself needs to be typeless
    // to allow different view formats (DSV and SRV).
    // DXGI_FORMAT_R24G8_TYPELESS is for D24_UNORM_S8_UINT depth.
    // DXGI_FORMAT_R32_TYPELESS is for D32_FLOAT depth.
    // Since your original was D24_UNORM_S8_UINT, we'll use R24G8_TYPELESS.
    descDepth.Format = DXGI_FORMAT_R24G8_TYPELESS;
    descDepth.SampleDesc.Count = 1u;
    descDepth.SampleDesc.Quality = 0u;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    // Add D3D11_BIND_SHADER_RESOURCE flag
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    descDepth.CPUAccessFlags = 0; // No CPU access needed
    descDepth.MiscFlags = 0;

    GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));

    // Create DSV (Depth Stencil View)
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Specific format for DSV
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    GFX_THROW_INFO(GetDevice(gfx)->CreateDepthStencilView(
        pDepthStencil.Get(), &descDSV, &pDepthStencilView
    ));

    // Create SRV (Shader Resource View) for reading the depth component
    D3D11_SHADER_RESOURCE_VIEW_DESC descSRV = {};
    descSRV.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; // Specific format for SRV (depth part)
    // For stencil, you'd use X24_TYPELESS_G8_UINT if needed
    descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    descSRV.Texture2D.MostDetailedMip = 0;
    descSRV.Texture2D.MipLevels = 1;
    GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(
        pDepthStencil.Get(), &descSRV, &pTextureView
    ));
}

void DepthStencil::BindAsDepthStencil(Graphics& gfx) const noexcept
{
    GetContext(gfx)->OMSetRenderTargets(0, nullptr, pDepthStencilView.Get());
}

void DepthStencil::Clear(Graphics& gfx) const noexcept
{
    GetContext(gfx)->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
}

// New method implementation
void DepthStencil::BindAsTexture(Graphics& gfx, UINT slot, bool forPixelShader) const noexcept
{
    if (pTextureView) // Check if SRV was created
    {
        if (forPixelShader)
        {
            GetContext(gfx)->PSSetShaderResources(slot, 1, pTextureView.GetAddressOf());
        }
        else
        {
            GetContext(gfx)->VSSetShaderResources(slot, 1, pTextureView.GetAddressOf());
        }
    }
}