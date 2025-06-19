#pragma once
#include "RenderQueuePass.h"
#include "Job.h"
#include <vector>
#include "Sink.h"
#include "Source.h"
#include "Stencil.h"
#include "ComparisonSampler.h"
#include "DepthStencil.h"

class Graphics;

namespace Rgph
{
	class LambertianPass : public RenderQueuePass
	{
	public:
		LambertianPass(Graphics& gfx, std::string name)
			: RenderQueuePass(std::move(name))
		{
			using namespace Bind;
			RegisterSink(DirectBufferSink<RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSink(DirectBufferSink<DepthStencil>::Make("depthStencil", depthStencil));
			AddBindSink<ShaderInputDepthStencil>("shadowMap");

			RegisterSource(DirectBufferSource<RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", depthStencil));

			AddBind(Stencil::Resolve(gfx, Stencil::Mode::Off));
			AddBind(std::make_shared<ComparisonSampler>(gfx));
		}
		void Execute(Graphics& gfx) const noxnd override
		{
			RenderQueuePass::Execute(gfx);
			ID3D11ShaderResourceView* const pNullSrv = nullptr;
			gfx.GetContext()->PSSetShaderResources(3, 1, &pNullSrv);
		}
	};
}