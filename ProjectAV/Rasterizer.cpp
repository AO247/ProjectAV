#include "Rasterizer.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind
{
	Rasterizer::Rasterizer(Graphics& gfx, bool twoSided)
		:
		Rasterizer(gfx, twoSided, 0, 0.0f, 0.0f) // Stary konstruktor wywo³uje nowy
	{
	}

	// --- DODAJ TEN KONSTRUKTOR ---
	Rasterizer::Rasterizer(Graphics& gfx, bool twoSided, int depthBias, float slopeBias, float clamp)
		:
		twoSided(twoSided),
		depthBias(depthBias),
		slopeBias(slopeBias),
		clamp(clamp)
	{
		INFOMAN(gfx);

		D3D11_RASTERIZER_DESC rasterDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
		rasterDesc.CullMode = twoSided ? D3D11_CULL_NONE : D3D11_CULL_BACK;

		// --- DODAJ TE USTAWIENIA ---
		rasterDesc.DepthBias = depthBias;
		rasterDesc.SlopeScaledDepthBias = slopeBias;
		rasterDesc.DepthBiasClamp = clamp;

		GFX_THROW_INFO(GetDevice(gfx)->CreateRasterizerState(&rasterDesc, &pRasterizer));
	}

	void Rasterizer::Bind(Graphics& gfx) noxnd
	{
		INFOMAN_NOHR(gfx);
		GFX_THROW_INFO_ONLY(GetContext(gfx)->RSSetState(pRasterizer.Get()));
	}

	std::shared_ptr<Rasterizer> Rasterizer::Resolve(Graphics& gfx, bool twoSided)
	{
		return Resolve(gfx, twoSided, 0, 0.0f, 0.0f);
	}

	// --- DODAJ TE FUNKCJE ---
	std::shared_ptr<Rasterizer> Rasterizer::Resolve(Graphics& gfx, bool twoSided, int depthBias, float slopeBias, float clamp)
	{
		return Codex::Resolve<Rasterizer>(gfx, twoSided, depthBias, slopeBias, clamp);
	}

	std::string Rasterizer::GenerateUID(bool twoSided)
	{
		return GenerateUID(twoSided, 0, 0.0f, 0.0f);
	}

	std::string Rasterizer::GenerateUID(bool twoSided, int depthBias, float slopeBias, float clamp)
	{
		using namespace std::string_literals;
		return typeid(Rasterizer).name() + "#"s + (twoSided ? "2s" : "1s") +
			"d"s + std::to_string(depthBias) +
			"s"s + std::to_string(slopeBias) +
			"c"s + std::to_string(clamp);
	}
	// --------------------------

	std::string Rasterizer::GetUID() const noexcept
	{
		return GenerateUID(twoSided, depthBias, slopeBias, clamp);
	}
}