#include "ComparisonSampler.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	ComparisonSampler::ComparisonSampler(Graphics& gfx)
	{
		INFOMAN(gfx);

		D3D11_SAMPLER_DESC sampDesc = {};
		sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.BorderColor[0] = 1.0f; // Poza map¹ cienia wszystko jest oœwietlone
		sampDesc.BorderColor[1] = 1.0f;
		sampDesc.BorderColor[2] = 1.0f;
		sampDesc.BorderColor[3] = 1.0f;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;

		GFX_THROW_INFO(GetDevice(gfx)->CreateSamplerState(&sampDesc, &pSampler));
	}

	void ComparisonSampler::Bind(Graphics& gfx) noxnd
	{
		INFOMAN_NOHR(gfx);
		// U¿ywamy slotu 1, tak jak zdefiniowaliœmy w Shadow.hlsl
		GetContext(gfx)->PSSetSamplers(1, 1, pSampler.GetAddressOf());
	}
}