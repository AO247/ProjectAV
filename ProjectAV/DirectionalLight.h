#pragma once
#include "Graphics.h"
#include "ConstantBuffers.h"

class DirectionalLight
{
public:
	DirectionalLight(Graphics& gfx, UINT slot = 0u);
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Bind(Graphics& gfx, DirectX::FXMMATRIX view) const noexcept;

private:
	struct DirectionalLightCBuf
	{
		DirectX::XMFLOAT3 direction;
		float padding1; // Dope�nienie do 16 bajt�w
		DirectX::XMFLOAT3 ambient;
		float padding2; // Dope�nienie do 16 bajt�w
		DirectX::XMFLOAT3 diffuseColor;
		float diffuseIntensity; // Idealnie dope�nia do 48 bajt�w
	};
public:
	DirectionalLightCBuf cbData;
	mutable Bind::PixelConstantBuffer<DirectionalLightCBuf> cbuf;
};