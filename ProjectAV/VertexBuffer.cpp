#include "VertexBuffer.h"
#include "BindableCodex.h"

namespace Bind
{
	VertexBuffer::VertexBuffer(Graphics& gfx, const Dvtx::VertexBuffer& vbuf)
		:
		VertexBuffer(gfx, "?", vbuf)
	{
	}
	ID3D11Buffer* VertexBuffer::Get() const noexcept
	{
		return pVertexBuffer.Get();
	}
	VertexBuffer::VertexBuffer(Graphics& gfx, const std::string& tag_in, const Dvtx::VertexBuffer& vbuf)
		:
		tag(tag_in),
		stride((UINT)vbuf.GetLayout().Size()), 
		layout_cache(vbuf.GetLayout()),      
		cpuDataBuffer(vbuf)                  
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT; 
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;

		bd.ByteWidth = UINT(cpuDataBuffer.SizeBytes());
		bd.StructureByteStride = stride;
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = cpuDataBuffer.GetData();
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bd, &sd, &pVertexBuffer));
	}

	const Dvtx::VertexLayout& VertexBuffer::GetLayout() const noexcept
	{
		return layout_cache; 
	}

	void VertexBuffer::Bind(Graphics& gfx) noxnd
	{
		const UINT offset = 0u;
		INFOMAN_NOHR(gfx);
		GFX_THROW_INFO_ONLY(GetContext(gfx)->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset));
	}


	size_t VertexBuffer::GetVertexCount() const noexcept
	{
		return cpuDataBuffer.Size(); 
	}

	Dvtx::ConstVertex VertexBuffer::GetVertex(size_t index) const noexcept
	{

		return cpuDataBuffer[index];
	}


	std::shared_ptr<VertexBuffer> VertexBuffer::Resolve(Graphics& gfx, const std::string& tag,
		const Dvtx::VertexBuffer& vbuf)
	{
		assert(tag != "?");
		return Codex::Resolve<VertexBuffer>(gfx, tag, vbuf);
	}
	std::string VertexBuffer::GenerateUID_(const std::string& tag)
	{
		using namespace std::string_literals;
		return typeid(VertexBuffer).name() + "#"s + tag;
	}
	std::string VertexBuffer::GetUID() const noexcept
	{
		return GenerateUID_(tag);
	}
}