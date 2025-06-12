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
			"Models\\Skybox\\right.bmp", "Models\\Skybox\\left.bmp",
			"Models\\Skybox\\up.bmp",   "Models\\Skybox\\down.bmp",
			"Models\\Skybox\\front.bmp", "Models\\Skybox\\back.bmp",
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