#include "ShadowMappingPass.h"
#include "Sink.h"
#include "Source.h"
#include "BindableCommon.h"
#include "RenderTarget.h"
#include "DepthStencil.h"

namespace Rgph
{
	ShadowMappingPass::ShadowMappingPass(Graphics& gfx, std::string name, unsigned int shadowMapDimension)
		:
		RenderQueuePass(std::move(name)),  
		m_shadowMapDimension(shadowMapDimension) 
	{
		using namespace Bind; 
		RegisterSink(DirectBufferSink<DepthStencil>::Make("depthStencil", depthStencil));
		 
		RegisterSource(DirectBindableSource<DepthStencil>::Make("depthStencil", depthStencil));

		 
		AddBind(NullPixelShader::Resolve(gfx));  
		AddBind(Stencil::Resolve(gfx, Stencil::Mode::Off));  
		AddBind(Rasterizer::Resolve(gfx, false, 50, 2.0f, 0.1f));
	} 
	void ShadowMappingPass::Execute(Graphics& gfx) const noxnd
	{
 
		D3D11_VIEWPORT vp = {};
		vp.Width = static_cast<float>(m_shadowMapDimension);
		vp.Height = static_cast<float>(m_shadowMapDimension);
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f; 
		gfx.GetContext()->RSSetViewports(1u, &vp);
		 
		RenderQueuePass::Execute(gfx);
		 
	}
}
