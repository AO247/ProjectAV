// BlurOutlineRenderGraph.cpp

#include "BlurOutlineRenderGraph.h"
#include "BufferClearPass.h"
#include "LambertianPass.h"
#include "OutlineDrawingPass.h"
#include "OutlineMaskGenerationPass.h"
#include "Source.h"
#include "HorizontalBlurPass.h"
#include "VerticalBlurPass.h"
#include "BlurOutlineDrawingPass.h"
#include "RenderTarget.h"
#include "DynamicConstant.h"
#include "CMath.h"
#include "ImGuiPass.h"
#include "ToneMappingPass.h"

namespace Rgph
{
	BlurOutlineRenderGraph::BlurOutlineRenderGraph(Graphics& gfx)
		:
		RenderGraph(gfx)
	{
		// MODIFIED: Initialize our new member variable here.
		// The result of make_shared is assigned to our std::shared_ptr<RenderTarget> member,
		// which is a valid upcast.
		pOffscreen = std::make_shared<Bind::ShaderInputRenderTarget>(gfx, gfx.GetWidth(), gfx.GetHeight(), 0);

		// MODIFIED: Now we pass our member variable 'pOffscreen' to the Make function.
		// Its type is std::shared_ptr<Bind::RenderTarget>, which is a perfect match for the
		// function's expected std::shared_ptr<Bind::RenderTarget>& parameter.
		AddGlobalSource(DirectBufferSource<Bind::RenderTarget>::Make("offscreen", pOffscreen));

		// Pass to clear the final back buffer
		{
			auto pass = std::make_unique<BufferClearPass>("clearRT");
			pass->SetSinkLinkage("buffer", "$.backbuffer");
			AppendPass(std::move(pass));
		}
		// Pass to clear the main depth stencil
		{
			auto pass = std::make_unique<BufferClearPass>("clearDS");
			pass->SetSinkLinkage("buffer", "$.masterDepth");
			AppendPass(std::move(pass));
		}
		// Pass to clear our new intermediate render target
		{
			auto pass = std::make_unique<BufferClearPass>("clearOffscreen");
			pass->SetSinkLinkage("buffer", "$.offscreen");
			AppendPass(std::move(pass));
		}
		// Main scene rendering pass
		{
			auto pass = std::make_unique<LambertianPass>(gfx, "lambertian");
			// LambertianPass now renders to our new, cleared "offscreen" buffer.
			pass->SetSinkLinkage("renderTarget", "clearOffscreen.buffer");
			pass->SetSinkLinkage("depthStencil", "clearDS.buffer");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<OutlineMaskGenerationPass>(gfx, "outlineMask");
			pass->SetSinkLinkage("depthStencil", "lambertian.depthStencil");
			AppendPass(std::move(pass));
		}

		// setup blur constant buffers (no change)
		{
			{
				Dcb::RawLayout l;
				l.Add<Dcb::Integer>("nTaps");
				l.Add<Dcb::Array>("coefficients");
				l["coefficients"].Set<Dcb::Float>(maxRadius * 2 + 1);
				Dcb::Buffer buf{ std::move(l) };
				blurKernel = std::make_shared<Bind::CachingPixelConstantBufferEx>(gfx, buf, 0);
				SetKernelGauss(radius, sigma);
				AddGlobalSource(DirectBindableSource<Bind::CachingPixelConstantBufferEx>::Make("blurKernel", blurKernel));
			}
			{
				Dcb::RawLayout l;
				l.Add<Dcb::Bool>("isHorizontal");
				Dcb::Buffer buf{ std::move(l) };
				blurDirection = std::make_shared<Bind::CachingPixelConstantBufferEx>(gfx, buf, 1);
				AddGlobalSource(DirectBindableSource<Bind::CachingPixelConstantBufferEx>::Make("blurDirection", blurDirection));
			}
		}

		// Tone Mapping Pass
		{
			auto pass = std::make_unique<ToneMappingPass>("toneMap", gfx);
			pass->SetSinkLinkage("scratchIn", "lambertian.renderTargetTexture");
			pass->SetSinkLinkage("renderTarget", "clearRT.buffer");
			AppendPass(std::move(pass));
		}

		{
			auto pass = std::make_unique<BlurOutlineDrawingPass>(gfx, "outlineDraw", gfx.GetWidth(), gfx.GetHeight());
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<HorizontalBlurPass>("horizontal", gfx, gfx.GetWidth(), gfx.GetHeight());
			pass->SetSinkLinkage("scratchIn", "outlineDraw.scratchOut");
			pass->SetSinkLinkage("kernel", "$.blurKernel");
			pass->SetSinkLinkage("direction", "$.blurDirection");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<VerticalBlurPass>("vertical", gfx);
			pass->SetSinkLinkage("renderTarget", "toneMap.renderTarget");
			pass->SetSinkLinkage("depthStencil", "outlineMask.depthStencil");
			pass->SetSinkLinkage("scratchIn", "horizontal.scratchOut");
			pass->SetSinkLinkage("kernel", "$.blurKernel");
			pass->SetSinkLinkage("direction", "$.blurDirection");
			AppendPass(std::move(pass));
		}

		SetSinkTarget("backbuffer", "vertical.renderTarget");

		Finalize();
	}

	void BlurOutlineRenderGraph::SetKernelGauss(int radius, float sigma) noxnd
	{
		assert(radius <= maxRadius);
		auto k = blurKernel->GetBuffer();
		const int nTaps = radius * 2 + 1;
		k["nTaps"] = nTaps;
		float sum = 0.0f;
		for (int i = 0; i < nTaps; i++)
		{
			const auto x = float(i - radius);
			const auto g = gauss(x, sigma);
			sum += g;
			k["coefficients"][i] = g;
		}
		for (int i = 0; i < nTaps; i++)
		{
			k["coefficients"][i] = (float)k["coefficients"][i] / sum;
		}
		blurKernel->SetBuffer(k);
	}
}