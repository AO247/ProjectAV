#pragma once
#include "Graphics.h"
#include "ConstantBuffers.h"
#include "ConditionalNoexcept.h"
#include <DirectXMath.h>

class DirectionalLight
{
public:
    DirectionalLight(Graphics& gfx);
    void Reset() noexcept;
    void Bind(Graphics& gfx, DirectX::FXMMATRIX view) const noexcept;

    void SetDirection(const DirectX::XMFLOAT3& dir) noexcept;
    void SetAmbient(const DirectX::XMFLOAT3& amb) noexcept;
    void SetDiffuseColor(const DirectX::XMFLOAT3& color) noexcept;
    void SetDiffuseIntensity(float intensity) noexcept;


private:

    struct DirectionalLightCBuf
    {
        alignas(16) DirectX::XMFLOAT3 direction;
        alignas(16) DirectX::XMFLOAT3 ambient;
        alignas(16) DirectX::XMFLOAT3 diffuseColor;
        float diffuseIntensity;
        float padding[3];
    };

private:
    DirectionalLightCBuf cbData;
    mutable Bind::PixelConstantBuffer<DirectionalLightCBuf> cbuf;
};