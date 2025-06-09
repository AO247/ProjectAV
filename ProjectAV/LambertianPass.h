#pragma once
#include "RenderQueuePass.h"
#include "Job.h"
#include <vector>
#include "Sink.h"
#include "Source.h"
#include "Stencil.h"

class Graphics;

namespace Rgph
{
	class LambertianPass : public RenderQueuePass
	{
	public:
		LambertianPass(Graphics& gfx, std::string name)
			:
			RenderQueuePass(std::move(name))
		{
			using namespace Bind;
			// REVERTED: The pass now sinks a render target again, just like it did originally.
			RegisterSink(DirectBufferSink<RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSink(DirectBufferSink<DepthStencil>::Make("depthStencil", depthStencil));

			// These sources are correct.
			RegisterSource(DirectBufferSource<RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSource(DirectBindableSource<RenderTarget>::Make("renderTargetTexture", renderTarget));
			RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", depthStencil));

			AddBind(Stencil::Resolve(gfx, Stencil::Mode::Off));
		}
	};
}