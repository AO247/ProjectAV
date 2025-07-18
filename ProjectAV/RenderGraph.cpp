#include "RenderGraph.h"
#include "Pass.h"
#include "DepthStencil.h"
#include "RenderTarget.h"
#include "BindableCommon.h"
#include "RenderGraphCompileException.h"
#include "RenderQueuePass.h"
#include "Sink.h"
#include "Source.h"
#include <sstream>

namespace Rgph
{
	RenderGraph::RenderGraph( Graphics& gfx )
		:
		backBufferTarget( gfx.GetTarget() ),
		masterDepth( std::make_shared<Bind::OutputOnlyDepthStencil>( gfx ) )
	{
		
		AddGlobalSource( DirectBufferSource<Bind::RenderTarget>::Make( "backbuffer",backBufferTarget ) );
		AddGlobalSource( DirectBufferSource<Bind::DepthStencil>::Make( "masterDepth",masterDepth ) );
		AddGlobalSink( DirectBufferSink<Bind::RenderTarget>::Make( "backbuffer",backBufferTarget ) );
	}

	Rgph::ParticlePass& RenderGraph::GetParticlePass()
	{
		throw RGC_EXCEPTION("GetParticlePass() not implemented in this RenderGraph class.");
	}

	RenderGraph::~RenderGraph()
	{}

	void RenderGraph::SetSinkTarget( const std::string& sinkName,const std::string& target )
	{
		const auto finder = [&sinkName]( const std::unique_ptr<Sink>& p ) {
			return p->GetRegisteredName() == sinkName;
		};
		const auto i = std::find_if( globalSinks.begin(),globalSinks.end(),finder );
		if( i == globalSinks.end() )
		{
			throw RGC_EXCEPTION( "Global sink does not exist: " + sinkName );
		}
		auto targetSplit = SplitString( target,"." );
		if( targetSplit.size() != 2u )
		{
			throw RGC_EXCEPTION( "Input target has incorrect format" );
		}
		(*i)->SetTarget( targetSplit[0],targetSplit[1] );
	}

	void RenderGraph::AddGlobalSource( std::unique_ptr<Source> out )
	{
		globalSources.push_back( std::move( out ) );
	}

	void RenderGraph::AddGlobalSink( std::unique_ptr<Sink> in )
	{
		globalSinks.push_back( std::move( in ) );
	}

	void RenderGraph::Execute( Graphics& gfx ) noxnd
	{
		assert( finalized );
		for( auto& p : passes )
		{
			p->Execute( gfx );
		}
	}

	void RenderGraph::Reset() noexcept
	{
		assert( finalized );
		for( auto& p : passes )
		{
			p->Reset();
		}
	}

	void RenderGraph::AppendPass( std::unique_ptr<Pass> pass )
	{
		assert( !finalized );
		
		for( const auto& p : passes )
		{
			if( pass->GetName() == p->GetName() )
			{
				throw RGC_EXCEPTION( "Pass name already exists: " + pass->GetName() );
			}
		}

		
		LinkSinks( *pass );


		passes.push_back( std::move( pass ) );
	}

	void RenderGraph::LinkSinks( Pass& pass )
	{
		for( auto& si : pass.GetSinks() )
		{
			const auto& inputSourcePassName = si->GetPassName();


			if( inputSourcePassName == "$" )
			{
				bool bound = false;
				for( auto& source : globalSources )
				{
					if( source->GetName() == si->GetOutputName() )
					{
						si->Bind( *source );
						bound = true;
						break;
					}
				}
				if( !bound )
				{
					std::ostringstream oss;
					oss << "Output named [" << si->GetOutputName() << "] not found in globals";
					throw RGC_EXCEPTION( oss.str() );
				}
			}
			else 
			{
				for( auto& existingPass : passes )
				{
					if( existingPass->GetName() == inputSourcePassName )
					{
						auto& source = existingPass->GetSource( si->GetOutputName() );
						si->Bind( source );
						break;
					}
				}
			}
		}
	}

	void RenderGraph::LinkGlobalSinks()
	{
		for( auto& sink : globalSinks )
		{
			const auto& inputSourcePassName = sink->GetPassName();
			for( auto& existingPass : passes )
			{
				if( existingPass->GetName() == inputSourcePassName )
				{
					auto& source = existingPass->GetSource( sink->GetOutputName() );
					sink->Bind( source );
					break;
				}
			}
		}
	}

	void RenderGraph::Finalize()
	{
		assert( !finalized );
		for( const auto& p : passes )
		{
			p->Finalize();
		}
		LinkGlobalSinks();
		finalized = true;
	}

	RenderQueuePass& RenderGraph::GetRenderQueue( const std::string& passName )
	{
		try
		{
			for( const auto& p : passes )
			{
				if( p->GetName() == passName )
				{
					return dynamic_cast<RenderQueuePass&>(*p);
				}
			}
		}
		catch( std::bad_cast& )
		{
			throw RGC_EXCEPTION( "In RenderGraph::GetRenderQueue, pass was not RenderQueuePass: " + passName );
		}
		throw RGC_EXCEPTION( "In RenderGraph::GetRenderQueue, pass not found: " + passName );
	}
}