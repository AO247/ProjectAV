#pragma once
#include "Graphics.h"
#include "ConstantBuffers.h"
#include <DirectXMath.h>
class Camera;
class DirectionalLight
{
public:
	DirectionalLight(Graphics& gfx, UINT slot = 0u);
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Bind(Graphics& gfx) const noexcept;
	void Update(Graphics& gfx, const DirectX::XMFLOAT3& playerWorldPosition, Camera& playerCamera);
	DirectX::XMMATRIX GetViewMatrix() const noexcept;
	DirectX::XMMATRIX GetProjectionMatrix() const noexcept;

private:
	// ZMIANA: Ta struktura teraz DOK�ADNIE odpowiada cbufferowi w HLSL
	struct DirectionalLightCBuf
	{
		DirectX::XMFLOAT3 viewLightDirection;
		float padding1;
		DirectX::XMFLOAT3 ambient;
		float padding2;
		DirectX::XMFLOAT3 diffuseColor;
		float diffuseIntensity;
	};
public:
	// To jest tylko szablon danych, kt�ry wype�niamy przed wys�aniem
	mutable DirectionalLightCBuf cbData;
	mutable Bind::PixelConstantBuffer<DirectionalLightCBuf> cbuf;

private:
	// NOWO��: Przechowujemy tu kierunek �wiat�a w przestrzeni �wiata
	DirectX::XMFLOAT3 worldDirection;
	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX projMatrix;
	float projWidth = 400.0f;
	float projHeight = 400.0f;
	float projNear = 1.0f;
	float projFar = 1000.0f;
};