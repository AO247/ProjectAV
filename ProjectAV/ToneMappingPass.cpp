#include "ToneMappingPass.h"
#include "PixelShader.h"
#include "RenderTarget.h"
#include "Blender.h"
#include "Sampler.h"
#include "DynamicConstant.h"

using namespace Bind;

namespace Rgph
{
	ToneMappingPass::ToneMappingPass(std::string name, Graphics& gfx)
		:
		FullscreenPass(std::move(name), gfx)
	{
		Dcb::RawLayout layout;
		layout.Add<Dcb::Float>("exposure");
		layout.Add<Dcb::Float>("sharpenAmount");
		layout.Add<Dcb::Float>("embossAmount");
		layout.Add<Dcb::Bool>("useSharpening");
		layout.Add<Dcb::Bool>("useEmboss");
		layout.Add<Dcb::Bool>("useOutline");
		layout.Add<Dcb::Float3>("outlineColor");
		layout.Add<Dcb::Float>("outlineThreshold");
		Dcb::Buffer buf(std::move(layout));

		buf["exposure"] = exposure;
		buf["sharpenAmount"] = sharpenAmount;
		buf["embossAmount"] = embossAmount;
		buf["useSharpening"] = useSharpening;
		buf["useEmboss"] = useEmboss;
		buf["useOutline"] = useOutline;
		buf["outlineColor"] = outlineColor;
		buf["outlineThreshold"] = outlineThreshold;

		pSettingsCBuf = std::make_shared<CachingPixelConstantBufferEx>(gfx, buf, 0u);
		AddBind(pSettingsCBuf);

		AddBind(PixelShader::Resolve(gfx, "ToneMap_PS.cso"));
		AddBind(Blender::Resolve(gfx, false));
		AddBind(Sampler::Resolve(gfx, Sampler::Type::Bilinear));

		AddBindSink<RenderTarget>("scratchIn");

		RegisterSink(DirectBufferSink<RenderTarget>::Make("renderTarget", renderTarget));
		RegisterSource(DirectBufferSource<RenderTarget>::Make("renderTarget", renderTarget));
	}

	void ToneMappingPass::Execute(Graphics& gfx) const noxnd
	{
		auto buf = pSettingsCBuf->GetBuffer();
		buf["exposure"] = exposure;
		buf["sharpenAmount"] = sharpenAmount;
		buf["embossAmount"] = embossAmount;
		buf["useSharpening"] = useSharpening;
		buf["useEmboss"] = useEmboss;
		buf["useOutline"] = useOutline;
		buf["outlineColor"] = outlineColor;
		buf["outlineThreshold"] = outlineThreshold;
		pSettingsCBuf->SetBuffer(buf);

		FullscreenPass::Execute(gfx);
	}
}