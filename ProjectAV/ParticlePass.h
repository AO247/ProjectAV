#pragma once
#include "Pass.h" // Change from RenderQueuePass
#include "Sink.h"
#include <vector>

class ParticleSystem;
class Graphics;

namespace Rgph
{
	class ParticlePass : public Pass
	{
	public:
		ParticlePass(Graphics& gfx, std::string name);
		void Execute(Graphics& gfx) const noxnd override;
		void Reset() noxnd override; // Add this
		void Accept(ParticleSystem* sys); // Change AddSystem to Accept
	private:
		std::vector<ParticleSystem*> systems;
		// Add these back from your original pass setup
		std::shared_ptr<Bind::RenderTarget> renderTarget;
		std::shared_ptr<Bind::DepthStencil> depthStencil;
	};
}