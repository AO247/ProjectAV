// In ParticlePass.h

#pragma once
#include "RenderQueuePass.h"
#include "Sink.h"
#include "Source.h"         // <--- Include Source.h for RegisterSource
#include "DepthStencil.h"
#include "RenderTarget.h"

namespace Rgph
{
	class ParticlePass : public RenderQueuePass
	{
	public:
		ParticlePass(Graphics& gfx, std::string name)
			:
			RenderQueuePass(std::move(name))
		{
			// Register the INPUTS that this pass requires.
			RegisterSink(DirectBufferSink<Bind::RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSink(DirectBufferSink<Bind::DepthStencil>::Make("depthStencil", depthStencil));

			// +++ ADD THIS MISSING SECTION +++
			// Register the OUTPUTS that this pass provides.
			// Since this pass just draws into the existing buffers without creating new ones,
			// we declare that our input buffers are also our outputs.
			RegisterSource(DirectBufferSource<Bind::RenderTarget>::Make("renderTarget", renderTarget));
			RegisterSource(DirectBufferSource<Bind::DepthStencil>::Make("depthStencil", depthStencil));
		}
	};
}