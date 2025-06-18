#include "MainRenderGraph.h"
#include "BufferClearPass.h"
#include "LambertianPass.h"
#include "SkyboxPass.h"       // Include all passes
#include "ParticlePass.h"
#include "Source.h"
#include "RenderTarget.h"
#include "DynamicConstant.h"
#include "imgui/imgui.h"
#include "CMath.h"

namespace Rgph
{
	// GetParticlePass implementation remains the same
	Rgph::ParticlePass& MainRenderGraph::GetParticlePass()
	{
		assert(pParticlePass != nullptr && "Particle pass has not been created or linked in MainRenderGraph");
		return *pParticlePass;
	}

	MainRenderGraph::MainRenderGraph(Graphics& gfx)
		:
		RenderGraph(gfx)
	{
		// 1. Clear the main render target and depth buffer
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

		// 2. Draw all solid, opaque geometry first. This populates the depth buffer.
		{
			auto pass = std::make_unique<LambertianPass>(gfx, "lambertian");
			pass->SetSinkLinkage("renderTarget", "clearRT.buffer");
			pass->SetSinkLinkage("depthStencil", "clearDS.buffer");
			AppendPass(std::move(pass));
		}

		// 3. Draw the skybox. It will correctly appear behind the solid geometry
		//    because it tests against the now-populated depth buffer.
		{
			auto pass = std::make_unique<SkyboxPass>(gfx, "skybox");
			// It reads from the output of the lambertian pass...
			pass->SetSinkLinkage("renderTarget", "lambertian.renderTarget");
			pass->SetSinkLinkage("depthStencil", "lambertian.depthStencil");
			AppendPass(std::move(pass));
		}

		// 4. Draw all transparent geometry (particles) last.
		{
			auto pass = std::make_unique<ParticlePass>(gfx, "particles");
			// It reads from the output of the SKYBOX pass. This means the skybox
			// is already in the color buffer, and particles will blend ON TOP of it.
			pass->SetSinkLinkage("renderTarget", "skybox.renderTarget");
			pass->SetSinkLinkage("depthStencil", "skybox.depthStencil");

			pParticlePass = static_cast<ParticlePass*>(pass.get());
			AppendPass(std::move(pass));
		}

		// 5. The final output of the graph is now the particle pass's render target.
		SetSinkTarget("backbuffer", "particles.renderTarget");

		Finalize();
	}
}