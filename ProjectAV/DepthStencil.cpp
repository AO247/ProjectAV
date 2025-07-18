#include "DepthStencil.h"
#include "RenderTarget.h"
#include "Graphics.h"
#include "GraphicsThrowMacros.h"

namespace wrl = Microsoft::WRL;

namespace Bind
{
	DepthStencil::DepthStencil(Graphics& gfx, UINT width, UINT height, bool canBindShaderInput)
	{
		INFOMAN(gfx);

		wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
		D3D11_TEXTURE2D_DESC descDepth = {};
		descDepth.Width = width;
		descDepth.Height = height;
		descDepth.MipLevels = 1u;
		descDepth.ArraySize = 1u;
		// ZMIANA: U�ywamy formatu bez-typowego, je�li chcemy czyta� tekstur� w shaderze
		descDepth.Format = canBindShaderInput ? DXGI_FORMAT_R24G8_TYPELESS : DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDepth.SampleDesc.Count = 1u;
		descDepth.SampleDesc.Quality = 0u;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | (canBindShaderInput ? D3D11_BIND_SHADER_RESOURCE : 0);
		GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));

		// ZMIANA: Jawnie tworzymy widok (View) z konkretnym formatem do zapisu g��bi
		D3D11_DEPTH_STENCIL_VIEW_DESC descView = {};
		descView.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descView.Flags = 0;
		descView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descView.Texture2D.MipSlice = 0;
		GFX_THROW_INFO(GetDevice(gfx)->CreateDepthStencilView(
			pDepthStencil.Get(), &descView, &pDepthStencilView
		));
	}

	void DepthStencil::BindAsBuffer(Graphics& gfx) noxnd
	{
		INFOMAN_NOHR(gfx);
		GFX_THROW_INFO_ONLY(GetContext(gfx)->OMSetRenderTargets(0, nullptr, pDepthStencilView.Get()));
	}

	void DepthStencil::BindAsBuffer(Graphics& gfx, BufferResource* renderTarget) noxnd
	{
		assert(dynamic_cast<RenderTarget*>(renderTarget) != nullptr);
		BindAsBuffer(gfx, static_cast<RenderTarget*>(renderTarget));
	}

	void DepthStencil::BindAsBuffer(Graphics& gfx, RenderTarget* rt) noxnd
	{
		rt->BindAsBuffer(gfx, this);
	}

	void DepthStencil::Clear(Graphics& gfx) noxnd
	{
		GetContext(gfx)->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
	}


	ShaderInputDepthStencil::ShaderInputDepthStencil(Graphics& gfx, UINT slot)
		:
		ShaderInputDepthStencil(gfx, gfx.GetWidth(), gfx.GetHeight(), slot)
	{
	}

	ShaderInputDepthStencil::ShaderInputDepthStencil(Graphics& gfx, UINT width, UINT height, UINT slot)
		:
		DepthStencil(gfx, width, height, true),
		slot(slot)
	{
		INFOMAN(gfx);

		wrl::ComPtr<ID3D11Resource> pRes;
		pDepthStencilView->GetResource(&pRes);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		// ZMIANA: U�ywamy formatu do odczytu, kt�ry ignoruje 8 bit�w stencila
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(
			pRes.Get(), &srvDesc, &pShaderResourceView
		));
	}

	void ShaderInputDepthStencil::Bind(Graphics& gfx) noxnd
	{
		INFOMAN_NOHR(gfx);
		GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetShaderResources(slot, 1u, pShaderResourceView.GetAddressOf()));
	}


	OutputOnlyDepthStencil::OutputOnlyDepthStencil(Graphics& gfx)
		:
		OutputOnlyDepthStencil(gfx, gfx.GetWidth(), gfx.GetHeight())
	{
	}

	OutputOnlyDepthStencil::OutputOnlyDepthStencil(Graphics& gfx, UINT width, UINT height)
		:
		DepthStencil(gfx, width, height, false)
	{
	}

	void OutputOnlyDepthStencil::Bind(Graphics& gfx) noxnd
	{
		assert("OutputOnlyDepthStencil cannot be bound as shader input" && false);
	}
}