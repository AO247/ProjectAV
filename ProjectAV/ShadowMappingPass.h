#pragma once
#include "RenderQueuePass.h"

namespace Rgph
{
	class ShadowMappingPass : public RenderQueuePass
	{
	public:
		ShadowMappingPass(Graphics& gfx, std::string name);
	};
}