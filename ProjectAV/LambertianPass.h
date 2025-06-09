// LambertianPass.h

#pragma once
#include "RenderQueuePass.h"
#include "Job.h"
#include <vector>
#include "Sink.h"
#include "Source.h"
#include "Stencil.h"
// NEW: Include RenderTarget header
#include "RenderTarget.h"

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
			// MODIFIED: This pass no longer receives a render target. It creates its own.
			// RegisterSink( DirectBufferSink<RenderTarget>::Make( "renderTarget",renderTarget ) );
			RegisterSink(DirectBufferSink<DepthStencil>::Make("depthStencil", depthStencil));

			// NEW: The pass creates its own internal ShaderInputRenderTarget.
			renderTarget = std::make_unique<ShaderInputRenderTarget>(gfx, gfx.GetWidth(), gfx.GetHeight(), 0);

			// MODIFIED: It now sources its own internal render target.
			RegisterSource(DirectBufferSource<RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSource(DirectBindableSource<RenderTarget>::Make("renderTargetTexture", renderTarget));
			RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", depthStencil));

			AddBind(Stencil::Resolve(gfx, Stencil::Mode::Off));
		}
	};
}