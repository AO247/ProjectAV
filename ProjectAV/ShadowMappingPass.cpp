#include "ShadowMappingPass.h"
#include "Sink.h"
#include "Source.h"
#include "BindableCommon.h"
#include "RenderTarget.h"
#include "DepthStencil.h"

namespace Rgph
{
	ShadowMappingPass::ShadowMappingPass(Graphics& gfx, std::string name)
		:
		RenderQueuePass(std::move(name))
	{
		using namespace Bind;

		RegisterSink(DirectBufferSink<DepthStencil>::Make("depthStencil", depthStencil));

		// ZMIANA: Zamiast DirectBufferSource, u�ywamy DirectBindableSource,
		// poniewa� nasz DepthStencil (kt�ry jest te� ShaderInputDepthStencil)
		// mo�e i musi by� bindowany do shadera w nast�pnym passie.
		RegisterSource(DirectBindableSource<DepthStencil>::Make("depthStencil", depthStencil));

		AddBind(NullPixelShader::Resolve(gfx));
		AddBind(Stencil::Resolve(gfx, Stencil::Mode::Off));
		AddBind(Rasterizer::Resolve(gfx, true, 50, 2.0f, 0.1f));
	}
}