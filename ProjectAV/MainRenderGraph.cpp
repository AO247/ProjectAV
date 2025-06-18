#include "MainRenderGraph.h"
#include "BufferClearPass.h"
#include "LambertianPass.h"
#include "Source.h"
#include "SkyboxPass.h"
#include "RenderTarget.h"
#include "DynamicConstant.h"
#include "imgui/imgui.h"
#include "CMath.h"

namespace Rgph
{

	Rgph::ParticlePass& MainRenderGraph::GetParticlePass()
	{
		// This assert is a good safety check. It will fire in debug mode
		// if something went wrong during construction and pParticlePass was not set.
		assert(pParticlePass != nullptr && "Particle pass has not been created or linked in MainRenderGraph");

		// Return a reference to the stored particle pass.
		return *pParticlePass;
	}

	MainRenderGraph::MainRenderGraph(Graphics& gfx)
		:
		RenderGraph(gfx)
	{
		{
			auto pass = std::make_unique<BufferClearPass>("clearRT");
			pass->SetSinkLinkage("buffer", "$.backbuffer");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<BufferClearPass>("clearDS");
			pass->SetSinkLinkage("buffer", "$.masterDepth");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<LambertianPass>(gfx, "lambertian");
			pass->SetSinkLinkage("renderTarget", "clearRT.buffer");
			pass->SetSinkLinkage("depthStencil", "clearDS.buffer");
			AppendPass(std::move(pass));
		}
		// +++ 3. ADD THE PARTICLE PASS to the graph +++
		{
			auto pass = std::make_unique<ParticlePass>(gfx, "particles");
			// Particles draw into the same target as solid objects
			pass->SetSinkLinkage("renderTarget", "lambertian.renderTarget");
			// They use the depth buffer from the solid pass for testing but don't write to it (handled by Blend state)
			pass->SetSinkLinkage("depthStencil", "lambertian.depthStencil");

			// Store the raw pointer so we can return it from GetParticlePass()
			pParticlePass = static_cast<ParticlePass*>(pass.get());
			// Add the pass to the graph's list (ownership is transferred)
			AppendPass(std::move(pass));
		}

		// 4. Draw the skybox last
		{
			auto pass = std::make_unique<SkyboxPass>(gfx, "skybox");
			pass->SetSinkLinkage("renderTarget", "particles.renderTarget");
			pass->SetSinkLinkage("depthStencil", "particles.depthStencil");
			AppendPass(std::move(pass));
		}
		SetSinkTarget("backbuffer", "skybox.renderTarget");

		Finalize();
	}
}
