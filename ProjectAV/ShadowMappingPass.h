#pragma once
#include "RenderQueuePass.h"
#include "Graphics.h"
namespace Rgph
{
	class ShadowMappingPass : public RenderQueuePass
	{
	public:
		ShadowMappingPass(Graphics& gfx, std::string name, unsigned int shadowMapDimension);
		void Execute(Graphics& gfx) const noxnd override;


	private:
		// ADD THIS: Member variable to store the shadow map dimension
		unsigned int m_shadowMapDimension;
	};
}