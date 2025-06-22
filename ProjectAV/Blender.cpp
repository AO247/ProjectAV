#include "Blender.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind
{
	// The NEW constructor. This is the most explicit and robust version.
	Blender::Blender(Graphics& gfx, Mode mode_in)
		:
		mode(mode_in)
	{
		INFOMAN(gfx);

		// Set legacy flags for UID generation compatibility
		blending = (mode != Mode::Normal);
		factors.reset();

		D3D11_BLEND_DESC blendDesc = {}; // Zero-initialize the description

		// --- Explicitly set top-level states to prevent pipeline state leakage ---
		// This ensures that we are using standard alpha blending and not some
		// other mode like alpha-to-coverage or logical operations.
		blendDesc.AlphaToCoverageEnable = FALSE;
		blendDesc.IndependentBlendEnable = FALSE; // Use the same blend state for all render targets

		auto& brt = blendDesc.RenderTarget[0];

		switch (mode)
		{
		case Mode::Alpha:
			brt.BlendEnable = TRUE;
			// Color: Final = (SrcColor * SrcAlpha) + (DestColor * (1 - SrcAlpha))
			brt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
			brt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			brt.BlendOp = D3D11_BLEND_OP_ADD;
			// Alpha: Final = (SrcAlpha * 1) + (DestAlpha * (1 - SrcAlpha))
			brt.SrcBlendAlpha = D3D11_BLEND_ONE;
			brt.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			brt.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			break;

		case Mode::Additive:
			brt.BlendEnable = TRUE;
			// Color: Final = (SrcColor * 1) + (DestColor * 1)
			brt.SrcBlend = D3D11_BLEND_ONE;
			brt.DestBlend = D3D11_BLEND_ONE;
			brt.BlendOp = D3D11_BLEND_OP_ADD;
			// Alpha: Final = (SrcAlpha * 1) + (DestAlpha * 1)
			brt.SrcBlendAlpha = D3D11_BLEND_ONE;
			brt.DestBlendAlpha = D3D11_BLEND_ONE;
			brt.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			break;

		case Mode::Normal:
		default:
			brt.BlendEnable = FALSE;
			break;
		}

		brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		GFX_THROW_INFO(GetDevice(gfx)->CreateBlendState(&blendDesc, &pBlender));
	}

	// The OLD constructor, for backward compatibility.
	Blender::Blender(Graphics& gfx, bool blending_in, std::optional<float> factor_in)
		:
		blending(blending_in)
	{
		INFOMAN(gfx);

		// Set the new 'mode' flag based on the legacy boolean.
		// This ensures GetUID() will work correctly.
		mode = blending ? Mode::Alpha : Mode::Normal;

		if (factor_in)
		{
			factors.emplace();
			factors->fill(*factor_in);
		}

		D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC{ CD3D11_DEFAULT{} };
		auto& brt = blendDesc.RenderTarget[0];
		if (blending)
		{
			brt.BlendEnable = TRUE;
			if (factor_in)
			{
				brt.SrcBlend = D3D11_BLEND_BLEND_FACTOR;
				brt.DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
			}
			else
			{
				brt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
				brt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			}
		}
		GFX_THROW_INFO(GetDevice(gfx)->CreateBlendState(&blendDesc, &pBlender));
	}

	void Blender::Bind(Graphics& gfx) noxnd
	{
		INFOMAN_NOHR(gfx);
		const float* data = factors ? factors->data() : nullptr;
		GFX_THROW_INFO_ONLY(GetContext(gfx)->OMSetBlendState(pBlender.Get(), data, 0xFFFFFFFFu));
	}

	void Blender::SetFactor(float factor) noxnd
	{
		assert(factors);
		factors->fill(factor);
	}
	float Blender::GetFactor() const noxnd
	{
		assert(factors);
		return factors->front();
	}

	std::shared_ptr<Blender> Blender::Resolve(Graphics& gfx, bool blending, std::optional<float> factor)
	{
		return Codex::Resolve<Blender>(gfx, blending, factor);
	}
	std::shared_ptr<Blender> Blender::Resolve(Graphics& gfx, Mode mode)
	{
		return Codex::Resolve<Blender>(gfx, mode);
	}

	std::string Blender::GenerateUID(bool blending, std::optional<float> factor)
	{
		using namespace std::string_literals;
		return typeid(Blender).name() + "#"s + (blending ? "b"s : "n"s) + (factor ? "#f"s + std::to_string(*factor) : "");
	}
	std::string Blender::GenerateUID(Mode mode)
	{
		using namespace std::string_literals;
		switch (mode)
		{
		case Mode::Alpha: return typeid(Blender).name() + "#alpha"s;
		case Mode::Additive: return typeid(Blender).name() + "#add"s;
		case Mode::Normal:
		default: return typeid(Blender).name() + "#normal"s;
		}
	}

	std::string Blender::GetUID() const noexcept
	{
		// If 'factors' was used, it's a legacy blend state, so use the legacy UID generator.
		if (factors)
		{
			return GenerateUID(blending, factors->front());
		}
		// Otherwise, it's a new blend state, so use the new UID generator.
		else
		{
			return GenerateUID(mode);
		}
	}
}