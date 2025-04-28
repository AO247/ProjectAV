#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(Graphics& gfx)
    :
    cbuf(gfx, 1u)
{
    Reset();
}

void DirectionalLight::Reset() noexcept
{
    cbData.direction = { 1.0f, 0.0f, 0.0f };

    cbData.ambient = { 0.1f, 0.1f, 0.1f };
    cbData.diffuseColor = { 1.0f, 1.0f, 1.0f };
    cbData.diffuseIntensity = 1.0f;
}

void DirectionalLight::SetDirection(const DirectX::XMFLOAT3& dir) noexcept
{
    DirectX::XMVECTOR dirVec = DirectX::XMLoadFloat3(&dir);

    if (DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(dirVec)) > 1e-6f)
    {
        DirectX::XMStoreFloat3(&cbData.direction, DirectX::XMVector3Normalize(dirVec));
    }
}

void DirectionalLight::SetAmbient(const DirectX::XMFLOAT3& amb) noexcept
{
    cbData.ambient = amb;
}

void DirectionalLight::SetDiffuseColor(const DirectX::XMFLOAT3& color) noexcept
{
    cbData.diffuseColor = color;
}

void DirectionalLight::SetDiffuseIntensity(float intensity) noexcept
{
    cbData.diffuseIntensity = intensity;
}


void DirectionalLight::Bind(Graphics& gfx, DirectX::FXMMATRIX view) const noexcept
{
    auto dataCopy = cbData;

    const auto dir = DirectX::XMLoadFloat3(&cbData.direction);

    const auto transformedDir = DirectX::XMVector3TransformNormal(dir, view);
    DirectX::XMStoreFloat3(&dataCopy.direction, DirectX::XMVector3Normalize(transformedDir));

   
    cbuf.Update(gfx, dataCopy);

    cbuf.Bind(gfx);
}