#include "SkyboxPass.h"
#include "Skybox.h"
#include "Graphics.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "Sink.h"
#include "Source.h"

namespace Rgph
{
	SkyboxPass::SkyboxPass(Graphics& gfx, std::string name)
		:
		BindingPass(std::move(name))
	{

		std::vector<std::string> facePaths = {
			"Models\\skybox2\\right.png", "Models\\skybox2\\left.png",
			"Models\\skybox2\\up.png",   "Models\\skybox2\\down.png",
			"Models\\skybox2\\front.png", "Models\\skybox2\\back.png",
		};
		pSkybox = std::make_unique<Skybox>(gfx, facePaths);

		RegisterSink(DirectBufferSink<Bind::RenderTarget>::Make("renderTarget", renderTarget));
		RegisterSink(DirectBufferSink<Bind::DepthStencil>::Make("depthStencil", depthStencil));
		RegisterSource(DirectBufferSource<Bind::RenderTarget>::Make("renderTarget", renderTarget));
		RegisterSource(DirectBufferSource<Bind::DepthStencil>::Make("depthStencil", depthStencil));
	}

	void SkyboxPass::Execute(Graphics& gfx) const noxnd
	{

		BindBufferResources(gfx);

		pSkybox->Draw(gfx);
	}
}