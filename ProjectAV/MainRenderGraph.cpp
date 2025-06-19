#include "MainRenderGraph.h"
#include "BufferClearPass.h"
#include "LambertianPass.h"
#include "SkyboxPass.h"
#include "ShadowMappingPass.h"
#include "Source.h"
#include "Sink.h"
#include "RenderTarget.h"
#include "DepthStencil.h"
#include "ComparisonSampler.h"

namespace Rgph
{
	MainRenderGraph::MainRenderGraph(Graphics& gfx)
		: RenderGraph(gfx)
	{
		// Inicjalizacja mapy cienia
		const unsigned int shadowMapSize = 2048;
		shadowMap = std::make_shared<Bind::ShaderInputDepthStencil>(gfx, shadowMapSize, shadowMapSize, 3); // slot 3

		AddGlobalSource(DirectBufferSource<Bind::BufferResource>::Make("shadowMap", shadowMap));

		{
			auto pass = std::make_unique<BufferClearPass>("clearShadowMap");
			pass->SetSinkLinkage("buffer", "$.shadowMap");
			AppendPass(std::move(pass));
		}
		// Pass 2: Renderuj scenê do mapy cienia
		{
			auto pass = std::make_unique<ShadowMappingPass>(gfx, "shadow");
			pass->SetSinkLinkage("depthStencil", "clearShadowMap.buffer");
			AppendPass(std::move(pass));
		}
		// Pass 3 & 4: Wyczyœæ g³ówny bufor ekranu i g³êbi
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
		// Pass 5: G³ówny pass renderuj¹cy (teraz z cieniem)
		{
			auto pass = std::make_unique<LambertianPass>(gfx, "lambertian");
			pass->SetSinkLinkage("renderTarget", "clearRT.buffer");
			pass->SetSinkLinkage("depthStencil", "clearDS.buffer");
			pass->SetSinkLinkage("shadowMap", "shadow.depthStencil");
			AppendPass(std::move(pass));
		}
		// Pass 6: Skybox
		{
			auto pass = std::make_unique<SkyboxPass>(gfx, "skybox");
			pass->SetSinkLinkage("renderTarget", "lambertian.renderTarget");
			pass->SetSinkLinkage("depthStencil", "lambertian.depthStencil");
			AppendPass(std::move(pass));
		}

		SetSinkTarget("backbuffer", "skybox.renderTarget");

		// ZMIANA: Ta linia zosta³a przeniesiona na górê
		// AddGlobalSource(DirectBufferSource<Bind::BufferResource>::Make("shadowMap", shadowMap));

		Finalize();
	}
}