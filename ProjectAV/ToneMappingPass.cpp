// ToneMappingPass.cpp

#include "ToneMappingPass.h"
#include "PixelShader.h"
#include "RenderTarget.h"
#include "Blender.h"
#include "Sampler.h"

using namespace Bind;

namespace Rgph
{
	ToneMappingPass::ToneMappingPass(std::string name, Graphics& gfx)
		:
		FullscreenPass(std::move(name), gfx)
	{
		AddBind(PixelShader::Resolve(gfx, "ToneMap_PS.cso"));
		AddBind(Blender::Resolve(gfx, false));
		AddBind(Sampler::Resolve(gfx, Sampler::Type::Bilinear));

		AddBindSink<RenderTarget>("scratchIn");

		// MODIFIED: This pass now receives the final render target instead of creating one.
		// renderTarget = std::make_shared<ShaderInputRenderTarget>(gfx, gfx.GetWidth(), gfx.GetHeight(), 0u);
		RegisterSink(DirectBufferSink<RenderTarget>::Make("renderTarget", renderTarget));
		RegisterSource(DirectBufferSource<RenderTarget>::Make("renderTarget", renderTarget));
	}

	void ToneMappingPass::Execute(Graphics& gfx) const noxnd
	{
		FullscreenPass::Execute(gfx);
	}
}