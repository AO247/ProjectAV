#define DVTX_SOURCE_FILE
#include "Vertex.h"

namespace Dvtx
{
	// VertexLayout
	const VertexLayout::Element& VertexLayout::ResolveByIndex(size_t i) const noxnd
	{
		return elements[i];
	}
	VertexLayout& VertexLayout::Append(ElementType type) noxnd
	{
		if (!Has(type)) // Only append if not already present for safety, though typical use is sequential appends
		{
			elements.emplace_back(type, Size());
		}
		return *this;
	}
	bool VertexLayout::Has(ElementType type) const noexcept
	{
		for (auto& e : elements)
		{
			if (e.GetType() == type)
			{
				return true;
			}
		}
		return false;
	}
	size_t VertexLayout::Size() const noxnd
	{
		return elements.empty() ? 0u : elements.back().GetOffsetAfter();
	}
	size_t VertexLayout::GetElementCount() const noexcept
	{
		return elements.size();
	}
	std::vector<D3D11_INPUT_ELEMENT_DESC> VertexLayout::GetD3DLayout() const noxnd
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> desc;
		desc.reserve(GetElementCount());
		for (const auto& e : elements)
		{
			desc.push_back(e.GetDesc());
		}
		return desc;
	}
	std::string VertexLayout::GetCode() const noxnd
	{
		std::string code;
		for (const auto& e : elements)
		{
			code += e.GetCode();
		}
		return code;
	}


	// VertexLayout::Element
	VertexLayout::Element::Element(ElementType type, size_t offset)
		:
		type(type),
		offset(offset)
	{
	}
	size_t VertexLayout::Element::GetOffsetAfter() const noxnd
	{
		return offset + Size();
	}
	size_t VertexLayout::Element::GetOffset() const
	{
		return offset;
	}
	size_t VertexLayout::Element::Size() const noxnd
	{
		return SizeOf(type);
	}
	VertexLayout::ElementType VertexLayout::Element::GetType() const noexcept
	{
		return type;
	}

	template<VertexLayout::ElementType type>
	struct SysSizeLookup
	{
		static constexpr auto Exec() noexcept
		{
			return sizeof(VertexLayout::Map<type>::SysType);
		}
	};
	constexpr size_t VertexLayout::Element::SizeOf(ElementType type) noxnd
	{
		return Bridge<SysSizeLookup>(type);
	}

	template<VertexLayout::ElementType type>
	struct CodeLookup
	{
		static constexpr auto Exec() noexcept
		{
			return VertexLayout::Map<type>::code;
		}
	};
	const char* Dvtx::VertexLayout::Element::GetCode() const noexcept
	{
		return Bridge<CodeLookup>(type);
	}

	template<VertexLayout::ElementType type> struct DescGenerate {
		static constexpr D3D11_INPUT_ELEMENT_DESC Exec(size_t offset) noexcept {
			return {
				VertexLayout::Map<type>::semantic,0,
				VertexLayout::Map<type>::dxgiFormat,
				0,(UINT)offset,D3D11_INPUT_PER_VERTEX_DATA,0
			};
		}
	};
	D3D11_INPUT_ELEMENT_DESC VertexLayout::Element::GetDesc() const noxnd
	{
		return Bridge<DescGenerate>(type, GetOffset());
	}


	// Vertex
	Vertex::Vertex(char* pData, const VertexLayout& layout) noxnd
		:
	pData(pData),
		layout(layout)
	{
		assert(pData != nullptr);
	}
	ConstVertex::ConstVertex(const Vertex& v) noxnd
		:
	vertex(v)
	{
	}


	// VertexBuffer
	VertexBuffer::VertexBuffer(VertexLayout layout, size_t size) noxnd
		:
	layout(std::move(layout))
	{
		Resize(size);
	}
	void VertexBuffer::Resize(size_t newSize) noxnd
	{
		const auto currentSize = Size(); // Use Size() which is element count
		if (layout.Size() == 0u && newSize > 0u) { // Prevent division by zero if layout is empty
			assert(false && "Cannot resize VertexBuffer with an empty layout to a non-zero size");
			return;
		}
		if (layout.Size() > 0u) { // Only resize if layout has elements
			buffer.resize(newSize * layout.Size());
		}
		else if (newSize == 0u) {
			buffer.clear();
		}
		// If layout.Size() is 0 and newSize > 0, it's an issue. Handled by assert.
	}
	const char* VertexBuffer::GetData() const noxnd
	{
		return buffer.data();
	}

	// Modified VertexBuffer constructor to handle general aiMesh data and specific bone data
	VertexBuffer::VertexBuffer(VertexLayout layout_in, const aiMesh& mesh, const std::vector<VertexBoneData>& vertexBoneDataSource)
		:
		layout(std::move(layout_in))
	{
		Resize(mesh.mNumVertices);
		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			auto currentVertex = (*this)[i]; // Get a Vertex proxy

			// Standard attributes
			if (layout.Has(VertexLayout::ElementType::Position3D) && mesh.mVertices) {
				currentVertex.Attr<VertexLayout::ElementType::Position3D>() = VertexLayout::Map<VertexLayout::ElementType::Position3D>::Extract(mesh, i);
			}
			if (layout.Has(VertexLayout::ElementType::Normal) && mesh.mNormals) {
				currentVertex.Attr<VertexLayout::ElementType::Normal>() = VertexLayout::Map<VertexLayout::ElementType::Normal>::Extract(mesh, i);
			}
			if (layout.Has(VertexLayout::ElementType::Texture2D) && mesh.HasTextureCoords(0) && mesh.mTextureCoords[0]) {
				currentVertex.Attr<VertexLayout::ElementType::Texture2D>() = VertexLayout::Map<VertexLayout::ElementType::Texture2D>::Extract(mesh, i);
			}
			else if (layout.Has(VertexLayout::ElementType::Texture2D)) { // Default if not present
				currentVertex.Attr<VertexLayout::ElementType::Texture2D>() = { 0.0f, 0.0f };
			}
			if (layout.Has(VertexLayout::ElementType::Tangent) && mesh.mTangents) {
				currentVertex.Attr<VertexLayout::ElementType::Tangent>() = VertexLayout::Map<VertexLayout::ElementType::Tangent>::Extract(mesh, i);
			}
			if (layout.Has(VertexLayout::ElementType::Bitangent) && mesh.mBitangents) {
				currentVertex.Attr<VertexLayout::ElementType::Bitangent>() = VertexLayout::Map<VertexLayout::ElementType::Bitangent>::Extract(mesh, i);
			}
			if (layout.Has(VertexLayout::ElementType::Float3Color) && mesh.HasVertexColors(0) && mesh.mColors[0]) {
				currentVertex.Attr<VertexLayout::ElementType::Float3Color>() = VertexLayout::Map<VertexLayout::ElementType::Float3Color>::Extract(mesh, i);
			}
			if (layout.Has(VertexLayout::ElementType::Float4Color) && mesh.HasVertexColors(0) && mesh.mColors[0]) {
				currentVertex.Attr<VertexLayout::ElementType::Float4Color>() = VertexLayout::Map<VertexLayout::ElementType::Float4Color>::Extract(mesh, i);
			}
			if (layout.Has(VertexLayout::ElementType::BGRAColor) && mesh.HasVertexColors(0) && mesh.mColors[0]) {
				currentVertex.Attr<VertexLayout::ElementType::BGRAColor>() = VertexLayout::Map<VertexLayout::ElementType::BGRAColor>::Extract(mesh, i);
			}


			// Bone attributes
			if (!vertexBoneDataSource.empty() && i < vertexBoneDataSource.size()) {
				const auto& boneData = vertexBoneDataSource[i];
				if (layout.Has(VertexLayout::ElementType::BoneIDs)) {
					currentVertex.Attr<VertexLayout::ElementType::BoneIDs>() = { boneData.ids[0], boneData.ids[1], boneData.ids[2], boneData.ids[3] };
				}
				if (layout.Has(VertexLayout::ElementType::BoneWeights)) {
					currentVertex.Attr<VertexLayout::ElementType::BoneWeights>() = { boneData.weights[0], boneData.weights[1], boneData.weights[2], boneData.weights[3] };
				}
			}
			else { // Default if no bone data provided but layout expects it
				if (layout.Has(VertexLayout::ElementType::BoneIDs)) {
					currentVertex.Attr<VertexLayout::ElementType::BoneIDs>() = { 0u,0u,0u,0u };
				}
				if (layout.Has(VertexLayout::ElementType::BoneWeights)) {
					currentVertex.Attr<VertexLayout::ElementType::BoneWeights>() = { 0.0f,0.0f,0.0f,0.0f };
				}
			}
		}
	}

	const VertexLayout& VertexBuffer::GetLayout() const noexcept
	{
		return layout;
	}
	size_t VertexBuffer::Size() const noxnd
	{
		if (layout.Size() == 0u) return 0u; // Prevent division by zero
		return buffer.size() / layout.Size();
	}
	size_t VertexBuffer::SizeBytes() const noxnd
	{
		return buffer.size();
	}
	Vertex VertexBuffer::Back() noxnd
	{
		assert(buffer.size() != 0u && layout.Size() > 0u);
		return Vertex{ buffer.data() + buffer.size() - layout.Size(),layout };
	}
	Vertex VertexBuffer::Front() noxnd
	{
		assert(buffer.size() != 0u && layout.Size() > 0u);
		return Vertex{ buffer.data(),layout };
	}
	Vertex VertexBuffer::operator[](size_t i) noxnd
	{
		assert(i < Size() && layout.Size() > 0u);
		return Vertex{ buffer.data() + layout.Size() * i,layout };
	}
	ConstVertex VertexBuffer::Back() const noxnd
	{
		return const_cast<VertexBuffer*>(this)->Back();
	}
	ConstVertex VertexBuffer::Front() const noxnd
	{
		return const_cast<VertexBuffer*>(this)->Front();
	}
	ConstVertex VertexBuffer::operator[](size_t i) const noxnd
	{
		return const_cast<VertexBuffer&>(*this)[i];
	}
}