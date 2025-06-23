#pragma once
#include "Bindable.h"

namespace Bind
{
	class ComparisonSampler : public Bindable
	{
	public:
		ComparisonSampler(Graphics& gfx);
		void Bind(Graphics& gfx) noxnd override;
	private:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	};
}