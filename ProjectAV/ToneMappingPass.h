#pragma once
#include "FullscreenPass.h"
#include "Sink.h"
#include "Source.h"
#include "ConstantBuffersEx.h"
#include <DirectXMath.h>

class Graphics;
namespace Bind
{
	class RenderTarget;
}

namespace Rgph
{
	class ToneMappingPass : public FullscreenPass
	{
	public:
		ToneMappingPass(std::string name, Graphics& gfx);
		void Execute(Graphics& gfx) const noxnd override;

		void SetAllParameters(float exposure, bool useSharpening, float sharpenAmount, bool useEmboss, float embossAmount, bool useOutline, DirectX::XMFLOAT3 outlineColor, float outlineThreshold)
		{
			this->exposure = exposure;
			this->useSharpening = useSharpening;
			this->sharpenAmount = sharpenAmount;
			this->useEmboss = useEmboss;
			this->embossAmount = embossAmount;
			this->useOutline = useOutline;
			this->outlineColor = outlineColor;
			this->outlineThreshold = outlineThreshold;
		}

	private:
		float exposure = 2.0f;
		float sharpenAmount = 10.0f;
		float embossAmount = 0.0f;
		bool useSharpening = true;
		bool useEmboss = true;
		bool useOutline = true;
		DirectX::XMFLOAT3 outlineColor = { 1.0f, 0.0f, 0.0f };
		float outlineThreshold = 0.01f;

		mutable std::shared_ptr<Bind::CachingPixelConstantBufferEx> pSettingsCBuf;
	};
}