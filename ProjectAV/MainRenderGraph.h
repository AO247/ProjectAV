#pragma once
#include "RenderGraph.h"
#include <memory>
#include "ConstantBuffersEx.h"

// Forward-declare Graphics, bo jest potrzebny w konstruktorze
class Graphics;

// ZMIANA: Zamiast niekompletnych deklaracji wyprzedzaj¹cych,
// do³¹czamy pe³ne definicje potrzebnych klas.
#include "RenderTarget.h"
#include "DepthStencil.h" // Ten plik zawiera definicjê BufferResource

namespace Rgph
{
	class MainRenderGraph : public RenderGraph
	{
	public:
		MainRenderGraph(Graphics& gfx);
		Rgph::ParticlePass& GetParticlePass() override;
	private:
		void SetKernelGauss(int radius, float sigma) noxnd;
		void SetKernelBox(int radius) noxnd;
		enum class KernelType
		{
			Gauss,
			Box,
		} kernelType = KernelType::Gauss;
		static constexpr int maxRadius = 7;
		int radius = 4;
		float sigma = 2.0f;
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> blurKernel;
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> blurDirection;

		// To pole jest teraz poprawne, bo kompilator wie, czym jest BufferResource
		std::shared_ptr<Bind::BufferResource> shadowMap;
		Rgph::ParticlePass* pParticlePass = nullptr;
	};
}