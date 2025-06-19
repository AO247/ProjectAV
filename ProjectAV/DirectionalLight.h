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
		float padding1; 
		DirectX::XMFLOAT3 ambient;
		float padding2; 
		DirectX::XMFLOAT3 diffuseColor;
		float diffuseIntensity; 
	};
public:
	DirectionalLightCBuf cbData;
	mutable Bind::PixelConstantBuffer<DirectionalLightCBuf> cbuf;
};