// BlurOutlineRenderGraph.h

#pragma once
#include "RenderGraph.h"
#include <memory>
#include "ConstantBuffersEx.h"
// NEW: Add this include for the forward declaration of the pass
#include "ToneMappingPass.h" 

class Graphics;
namespace Bind
{
	class Bindable;
	class RenderTarget; // We will use shared_ptr<RenderTarget>, so forward declaration is fine
}

namespace Rgph
{
	class BlurOutlineRenderGraph : public RenderGraph
	{
	public:
		BlurOutlineRenderGraph(Graphics& gfx);
	private:
		// private functions
		void SetKernelGauss(int radius, float sigma) noxnd;
		// private data
		static constexpr int maxRadius = 7;
		static constexpr int radius = 4;
		static constexpr float sigma = 2.0f;
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> blurKernel;
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> blurDirection;

		// =======================================================================
		// NEW: Add a member variable to own the intermediate offscreen buffer.
		// It MUST be std::shared_ptr<Bind::RenderTarget> to match the Make function.
		std::shared_ptr<Bind::RenderTarget> pOffscreen;
		// =======================================================================
	};
}