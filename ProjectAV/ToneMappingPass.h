#pragma once
#include "FullscreenPass.h"
#include "Sink.h"
#include "Source.h"

class Graphics;
namespace Bind
{
	class RenderTarget;
	class DepthStencil;
}

namespace Rgph
{
	class ToneMappingPass : public FullscreenPass
	{
	public:
		ToneMappingPass(std::string name, Graphics& gfx);
		void Execute(Graphics& gfx) const noxnd override;
	};
}