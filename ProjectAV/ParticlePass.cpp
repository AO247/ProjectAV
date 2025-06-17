#include "ParticlePass.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "Sink.h"
#include "Source.h"
#include "Particle.h"
#include "Graphics.h"

namespace Rgph
{
	ParticlePass::ParticlePass(Graphics& gfx, std::string name)
		:
		Pass(std::move(name))
	{
		RegisterSink(DirectBufferSink<Bind::RenderTarget>::Make("renderTarget", renderTarget));
		RegisterSink(DirectBufferSink<Bind::DepthStencil>::Make("depthStencil", depthStencil));
		RegisterSource(DirectBufferSource<Bind::RenderTarget>::Make("renderTarget", renderTarget));
		RegisterSource(DirectBufferSource<Bind::DepthStencil>::Make("depthStencil", depthStencil));
	}

	void ParticlePass::Accept(ParticleSystem* sys)
	{
		systems.push_back(sys);
	}

	// This gets called by RenderGraph::Reset()
	void ParticlePass::Reset() noxnd
	{
		systems.clear();
	}

	void ParticlePass::Execute(Graphics& gfx) const noxnd
	{
		renderTarget->BindAsBuffer(gfx, depthStencil.get());

		for (const auto sys : systems)
		{
			sys->Bind(gfx);
			gfx.DrawIndexedInstanced(sys->GetIndexCount(), sys->GetInstanceCount());
		}
	}
}