#pragma once
#include "BindingPass.h"
#include <memory>

class Skybox;

namespace Rgph
{
	class SkyboxPass : public BindingPass
	{
	public:
		SkyboxPass(Graphics& gfx, std::string name);
		void Execute(Graphics& gfx) const noxnd override;
	private:
		std::unique_ptr<Skybox> pSkybox;
	};
}