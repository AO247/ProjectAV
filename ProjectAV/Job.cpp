#include "Job.h"

namespace Rgph
{
	// Update the constructor implementation
	Job::Job(std::function<void(Graphics&)> job)
		:
		pJob(std::move(job))
	{
	}

	// Update the Execute implementation
	void Job::Execute(Graphics& gfx) const noxnd
	{
		pJob(gfx);
	}
}