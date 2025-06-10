#include "IndexBuffer.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"
#include <cassert> // For assert

namespace Bind
{
	IndexBuffer::IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices_in)
		:
		IndexBuffer(gfx, "?", indices_in) // Delegate to tagged constructor
	{
	}

	IndexBuffer::IndexBuffer(Graphics& gfx, std::string tag_in, const std::vector<unsigned short>& indices_in)
		:
		tag(tag_in),
		count((UINT)indices_in.size()),
		cpuIndices(indices_in) // **** Store a copy of the input indices ****
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC ibd = {};
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.Usage = D3D11_USAGE_DEFAULT;
		ibd.CPUAccessFlags = 0u;
		ibd.MiscFlags = 0u;
		ibd.ByteWidth = UINT(count * sizeof(unsigned short));
		ibd.StructureByteStride = sizeof(unsigned short);
		D3D11_SUBRESOURCE_DATA isd = {};
		// Use the original passed-in indices_in.data() for GPU upload for consistency
		// or cpuIndices.data() if you prefer. Both hold the same data at this point.
		isd.pSysMem = indices_in.data();
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&ibd, &isd, &pIndexBuffer));
	}

	void IndexBuffer::Bind(Graphics& gfx) noxnd
	{
		INFOMAN_NOHR(gfx);
		GFX_THROW_INFO_ONLY(GetContext(gfx)->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u));
	}

	UINT IndexBuffer::GetCount() const noexcept
	{
		return count; // This is already the number of indices
	}

	// **** IMPLEMENTATION OF NEW PUBLIC METHODS ****
	unsigned short IndexBuffer::GetIndex(size_t i) const noexcept
	{
		assert(i < cpuIndices.size() && "Index out of bounds in IndexBuffer::GetIndex");
		return cpuIndices[i];
	}

	/* Optional:
	const std::vector<unsigned short>& IndexBuffer::GetCPUIndices() const noexcept
	{
		return cpuIndices;
	}
	*/

	std::shared_ptr<IndexBuffer> IndexBuffer::Resolve(Graphics& gfx, const std::string& tag,
		const std::vector<unsigned short>& indices)
	{
		assert(tag != "?"); // Make sure tag is not the default placeholder
		return Codex::Resolve<IndexBuffer>(gfx, tag, indices);
	}
	std::string IndexBuffer::GenerateUID_(const std::string& tag)
	{
		using namespace std::string_literals;
		return typeid(IndexBuffer).name() + "#"s + tag;
	}
	std::string IndexBuffer::GetUID() const noexcept
	{
		// Call the static GenerateUID_ with the instance's tag
		return GenerateUID_(tag);
	}
}