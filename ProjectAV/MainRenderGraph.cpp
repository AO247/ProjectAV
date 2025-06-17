#include "MainRenderGraph.h"
#include "BufferClearPass.h"
#include "LambertianPass.h"
#include "Source.h"
#include "SkyboxPass.h"
#include "RenderTarget.h"
#include "DynamicConstant.h"
#include "imgui/imgui.h"
#include "CMath.h"
#include "ParticlePass.h"

namespace Rgph
{
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
		{
			auto pass = std::make_unique<SkyboxPass>(gfx, "skybox");
			pass->SetSinkLinkage("renderTarget", "lambertian.renderTarget");
			pass->SetSinkLinkage("depthStencil", "lambertian.depthStencil");
			AppendPass(std::move(pass));
		}
		// Add the particle pass. It will be empty at first each frame.
		{
			auto pass = std::make_unique<ParticlePass>(gfx, "particles");
			pass->SetSinkLinkage("renderTarget", "skybox.renderTarget");
			pass->SetSinkLinkage("depthStencil", "skybox.depthStencil");
			AppendPass(std::move(pass));
		}

		SetSinkTarget("backbuffer", "particles.renderTarget");
		Finalize();
	}
}
