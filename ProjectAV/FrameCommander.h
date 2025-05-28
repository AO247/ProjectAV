#pragma once
#include <array>
#include "BindableCommon.h"
#include "Graphics.h"
#include "Job.h"
#include "Pass.h"
#include "PerfLog.h"
#include "DepthStencil.h"
#include "RenderTarget.h"
#include "BlurPack.h"
#include <array>

struct TimeBuffer
{
	float time;
	float padding[3]; 
};

class FrameCommander
{
public:
	FrameCommander( Graphics& gfx )
		:
		ds( gfx,gfx.GetWidth(),gfx.GetHeight() ),
		rt1( { gfx,gfx.GetWidth() / downFactor,gfx.GetHeight() / downFactor } ),
		rt2( { gfx,gfx.GetWidth() / downFactor,gfx.GetHeight() / downFactor } ),
		blur( gfx,15,100.6f,"BlurOutline_PS.cso" )
	{
		namespace dx = DirectX;

		// setup fullscreen geometry
		Dvtx::VertexLayout lay;
		lay.Append( Dvtx::VertexLayout::Position2D );
		Dvtx::VertexBuffer bufFull{ lay };
		bufFull.EmplaceBack( dx::XMFLOAT2{ -1,1 } );
		bufFull.EmplaceBack( dx::XMFLOAT2{ 1,1 } );
		bufFull.EmplaceBack( dx::XMFLOAT2{ -1,-1 } );
		bufFull.EmplaceBack( dx::XMFLOAT2{ 1,-1 } );
		pVbFull = Bind::VertexBuffer::Resolve( gfx,"$Full",std::move( bufFull ) );
		std::vector<unsigned short> indices = { 0,1,2,1,3,2 };
		pIbFull = Bind::IndexBuffer::Resolve( gfx,"$Full",std::move( indices ) );

		// setup fullscreen shaders
		pVsFull = Bind::VertexShader::Resolve( gfx,"Fullscreen_VS.cso" );
		pLayoutFull = Bind::InputLayout::Resolve( gfx,lay,pVsFull->GetBytecode() );
		pSamplerFullPoint = Bind::Sampler::Resolve( gfx,false,true );
		pSamplerFullBilin = Bind::Sampler::Resolve( gfx,true,true );
		pBlenderMerge = Bind::Blender::Resolve( gfx,true );

		Dvtx::VertexLayout layWT;
		layWT.Append(Dvtx::VertexLayout::Position2D);
		layWT.Append(Dvtx::VertexLayout::Texture2D);
		Dvtx::VertexBuffer bufWT{ layWT };
		bufWT.EmplaceBack(dx::XMFLOAT2{ -1, 1 }, dx::XMFLOAT2{ 0, 0 });
		bufWT.EmplaceBack(dx::XMFLOAT2{ 1, 1 }, dx::XMFLOAT2{ 1, 0 });
		bufWT.EmplaceBack(dx::XMFLOAT2{ -1,-1 }, dx::XMFLOAT2{ 0, 1 });
		bufWT.EmplaceBack(dx::XMFLOAT2{ 1,-1 }, dx::XMFLOAT2{ 1, 1 });
		pVbWindTunnel = Bind::VertexBuffer::Resolve(gfx, "$WindTunnel", std::move(bufWT));
		std::vector<unsigned short> indicesWT = { 0,1,2,1,3,2 };
		pIbWindTunnel = Bind::IndexBuffer::Resolve(gfx, "$WindTunnel", std::move(indicesWT));

		// Shaders
		pVsWindTunnel = Bind::VertexShader::Resolve(gfx, "WindTunnel_VS.cso");
		pPsWindTunnel = Bind::PixelShader::Resolve(gfx, "WindTunnel_PS.cso");
		pLayoutWindTunnel = Bind::InputLayout::Resolve(gfx, layWT, pVsWindTunnel->GetBytecode());

		// Constant buffer (czas)
		TimeBuffer tb = { 0.0f };
		pCbTimeWindTunnel = std::make_shared<Bind::PixelConstantBuffer<TimeBuffer>>(gfx, tb, 0u);
	}
	void Accept( Job job,size_t target ) noexcept
	{
		passes[target].Accept( job );
	}
	void Execute( Graphics& gfx ) noxnd
	{
		using namespace Bind;
		// normally this would be a loop with each pass defining its setup / etc.
		// and later on it would be a complex graph with parallel execution contingent
		// on input / output requirements

		// setup render target used for normal passes
		ds.Clear( gfx );
		rt1->Clear( gfx );
		gfx.BindSwapBuffer( ds );
		// main phong lighting pass
		Blender::Resolve( gfx,false )->Bind( gfx );
		Stencil::Resolve( gfx,Stencil::Mode::Off )->Bind( gfx );
		passes[0].Execute( gfx );
		// outline masking pass
		Stencil::Resolve( gfx,Stencil::Mode::Write )->Bind( gfx );
		NullPixelShader::Resolve( gfx )->Bind( gfx );
		passes[1].Execute( gfx );
		// outline drawing pass
		rt1->BindAsTarget( gfx );
		Stencil::Resolve( gfx,Stencil::Mode::Off )->Bind( gfx );
		passes[2].Execute( gfx );
		// fullscreen blur h-pass
		rt2->BindAsTarget( gfx );
		rt1->BindAsTexture( gfx,0 );
		pVbFull->Bind( gfx );
		pIbFull->Bind( gfx );
		pVsFull->Bind( gfx );
		pLayoutFull->Bind( gfx );
		pSamplerFullPoint->Bind( gfx );
		blur.Bind( gfx );
		blur.SetHorizontal( gfx );
		gfx.DrawIndexed( pIbFull->GetCount() );
		// fullscreen blur v-pass + combine
		gfx.BindSwapBuffer( ds );
		rt2->BindAsTexture( gfx,0u );
		pBlenderMerge->Bind( gfx );
		pSamplerFullBilin->Bind( gfx );
		Stencil::Resolve( gfx,Stencil::Mode::Mask )->Bind( gfx );
		blur.SetVertical( gfx );
		gfx.DrawIndexed( pIbFull->GetCount() );

			static float currentTime = 0.0f;
		currentTime += 0.016f; // Simulate ~60 FPS
		SetWindTunnelShader(currentTime*2, gfx);
		DrawFullscreenQuad(gfx);
	}
	void Reset() noexcept
	{
		for( auto& p : passes )
		{
			p.Reset();
		}
	}
	void ShowWindows( Graphics& gfx )
	{
		if( ImGui::Begin( "Blur" ) )
		{
			if( ImGui::SliderInt( "Down Factor",&downFactor,1,16 ) )
			{
				rt1.emplace( gfx,gfx.GetWidth() / downFactor,gfx.GetHeight() / downFactor );
				rt2.emplace( gfx,gfx.GetWidth() / downFactor,gfx.GetHeight() / downFactor );
			}
			blur.RenderWidgets( gfx );
		}
		ImGui::End();
	}

	void SetWindTunnelShader(float time, Graphics& gfx)
	{
		TimeBuffer tb = { time };
		pCbTimeWindTunnel->Update(gfx, tb);

		pVbWindTunnel->Bind(gfx);
		pIbWindTunnel->Bind(gfx);
		pVsWindTunnel->Bind(gfx);
		pPsWindTunnel->Bind(gfx);
		pLayoutWindTunnel->Bind(gfx);
		pCbTimeWindTunnel->Bind(gfx);
	}

	void DrawFullscreenQuad(Graphics& gfx)
	{
		gfx.DrawIndexed(pIbWindTunnel->GetCount());
	}

private:
	std::array<Pass,3> passes;
	int downFactor = 1;
	DepthStencil ds;
	std::optional<RenderTarget> rt1;
	std::optional<RenderTarget> rt2;
	BlurPack blur;
	std::shared_ptr<Bind::VertexBuffer> pVbFull;
	std::shared_ptr<Bind::IndexBuffer> pIbFull;
	std::shared_ptr<Bind::VertexShader> pVsFull;
	std::shared_ptr<Bind::InputLayout> pLayoutFull;
	std::shared_ptr<Bind::Sampler> pSamplerFullPoint;
	std::shared_ptr<Bind::Sampler> pSamplerFullBilin;
	std::shared_ptr<Bind::Blender> pBlenderMerge;
	std::shared_ptr<Bind::VertexShader> pVsWindTunnel;
	std::shared_ptr<Bind::PixelShader> pPsWindTunnel;
	std::shared_ptr<Bind::InputLayout> pLayoutWindTunnel;
	std::shared_ptr<Bind::VertexBuffer> pVbWindTunnel;
	std::shared_ptr<Bind::IndexBuffer> pIbWindTunnel;
	std::shared_ptr<Bind::ConstantBuffer<TimeBuffer>> pCbTimeWindTunnel;
};