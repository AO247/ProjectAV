#pragma once
#include "Bindable.h"
#include <array>

namespace Bind
{
	class Rasterizer : public Bindable
	{
	public:
		Rasterizer(Graphics& gfx, bool twoSided);
		// --- DODAJ TEN KONSTRUKTOR ---
		Rasterizer(Graphics& gfx, bool twoSided, int depthBias, float slopeBias, float clamp);
		void Bind(Graphics& gfx) noxnd override;
		static std::shared_ptr<Rasterizer> Resolve(Graphics& gfx, bool twoSided, int depthBias, float slopeBias, float clamp);
		static std::shared_ptr<Rasterizer> Resolve(Graphics& gfx, bool twoSided);
		static std::string GenerateUID(bool twoSided, int depthBias, float slopeBias, float clamp);
		static std::string GenerateUID(bool twoSided);
		std::string GetUID() const noexcept override;
	protected:
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
		bool twoSided;
		// --- DODAJ TE POLA ---
		int depthBias;
		float slopeBias;
		float clamp;
	};
}