#include "VertexBuffer.h"
#include "BindableCodex.h"

namespace Bind
{
	VertexBuffer::VertexBuffer(Graphics& gfx, const Dvtx::VertexBuffer& vbuf)
		:
		VertexBuffer(gfx, "?", vbuf) // Delegate to the tagged constructor
	{
	}

	VertexBuffer::VertexBuffer(Graphics& gfx, const std::string& tag_in, const Dvtx::VertexBuffer& vbuf)
		:
		tag(tag_in),
		stride((UINT)vbuf.GetLayout().Size()), // Stride from incoming vbuf's layout
		layout_cache(vbuf.GetLayout()),      // Cache the layout
		cpuDataBuffer(vbuf)                  // <<<< Store a copy of the CPU-side Dvtx::VertexBuffer
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT; // Or D3D11_USAGE_IMMUTABLE if it never changes post-creation
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		// Use the stored cpuDataBuffer for size and data pointer
		bd.ByteWidth = UINT(cpuDataBuffer.SizeBytes());
		bd.StructureByteStride = stride; // Stride is per-vertex, ByteWidth is total
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = cpuDataBuffer.GetData();
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bd, &sd, &pVertexBuffer));
	}

	const Dvtx::VertexLayout& VertexBuffer::GetLayout() const noexcept
	{
		return layout_cache; // Return the cached layout
		// Or: return cpuDataBuffer.GetLayout(); if you remove layout_cache
	}

	void VertexBuffer::Bind(Graphics& gfx) noexcept
	{
		const UINT offset = 0u;
		GetContext(gfx)->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
	}

	// **** IMPLEMENTATION OF NEW METHODS ****
	size_t VertexBuffer::GetVertexCount() const noexcept
	{
		return cpuDataBuffer.Size(); // Dvtx::VertexBuffer::Size() returns number of vertices
	}

	Dvtx::ConstVertex VertexBuffer::GetVertex(size_t index) const noexcept
	{
		// Dvtx::VertexBuffer::operator[] should handle assertions for out-of-bounds
		return cpuDataBuffer[index];
	}

	/* Optional:
	const Dvtx::VertexBuffer& VertexBuffer::GetCPUDataBuffer() const noexcept
	{
		return cpuDataBuffer;
	}
	*/

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
		return GenerateUID_(tag); // Assuming GenerateUID_ is meant to be called like this
	}
}