#include "DxgiInfoManager.h"
#include "Window.h"
#include "Graphics.h"
#include <dxgidebug.h>
#include <memory>
#include "GraphicsThrowMacros.h"
#include "WindowsThrowMacros.h"

#pragma comment(lib, "dxguid.lib")


DxgiInfoManager::DxgiInfoManager()
{

	typedef HRESULT (WINAPI* DXGIGetDebugInterface)(REFIID,void **);


	const auto hModDxgiDebug = LoadLibraryEx( "dxgidebug.dll",nullptr,LOAD_LIBRARY_SEARCH_SYSTEM32 );
	if( hModDxgiDebug == nullptr )
	{
		throw CHWND_LAST_EXCEPT();
	}


	const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
		reinterpret_cast<void*>(GetProcAddress( hModDxgiDebug,"DXGIGetDebugInterface" ))
	);
	if( DxgiGetDebugInterface == nullptr )
	{
		throw CHWND_LAST_EXCEPT();
	}

	HRESULT hr;
	GFX_THROW_NOINFO( DxgiGetDebugInterface( __uuidof(IDXGIInfoQueue),&pDxgiInfoQueue ) );
}

void DxgiInfoManager::Set() noexcept
{

	next = pDxgiInfoQueue->GetNumStoredMessages( DXGI_DEBUG_ALL );
}

std::vector<std::string> DxgiInfoManager::GetMessages() const
{
	std::vector<std::string> messages;
	const auto end = pDxgiInfoQueue->GetNumStoredMessages( DXGI_DEBUG_ALL );
	for( auto i = next; i < end; i++ )
	{
		HRESULT hr;
		SIZE_T messageLength;

		GFX_THROW_NOINFO( pDxgiInfoQueue->GetMessage( DXGI_DEBUG_ALL,i,nullptr,&messageLength ) );

		auto bytes = std::make_unique<byte[]>( messageLength );
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());

		GFX_THROW_NOINFO( pDxgiInfoQueue->GetMessage( DXGI_DEBUG_ALL,i,pMessage,&messageLength ) );
		messages.emplace_back( pMessage->pDescription );
	}
	return messages;
}
