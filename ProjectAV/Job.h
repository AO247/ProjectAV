#pragma once
#include "ConditionalNoexcept.h"
#include <functional> // <-- Include for std::function

class Graphics;

namespace Rgph
{
	class Job
	{
	public:
		// Change the constructor to take a generic std::function
		Job(std::function<void(Graphics&)> job);
		void Execute(Graphics& gfx) const noxnd;
	private:
		std::function<void(Graphics&)> pJob;
	};
}