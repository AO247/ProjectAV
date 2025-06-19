#define DVTX_SOURCE_FILE
#include "Vertex.h"
#include <algorithm> // For std::find_if

namespace Dvtx
{

	// ... (The VertexLayout and Element implementation is unchanged) ...
	const VertexLayout::Element& VertexLayout::ResolveByIndex(size_t i) const noxnd
	{
		return elements[i];
	}
	VertexLayout& VertexLayout::Append(ElementType type) noxnd
	{
		if (!Has(type))
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
			// This will use a semantic index of 0, which is what we need for
			// BLENDINDICES0 and BLENDWEIGHTS0.
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



	VertexBuffer::VertexBuffer(VertexLayout layout, size_t size) noxnd
		:
	layout(std::move(layout))
	{
		Resize(size);
	}
	void VertexBuffer::Resize(size_t newSize) noxnd
	{
		const auto size = Size();
		if (size < newSize)
		{
			buffer.resize(buffer.size() + layout.Size() * (newSize - size));
		}
	}
	const char* VertexBuffer::GetData() const noxnd
	{
		return buffer.data();
	}

	//================================================================================
	// REWRITTEN CONSTRUCTOR to handle complex data like bone weights
	//================================================================================
	VertexBuffer::VertexBuffer(VertexLayout layout_in, const aiMesh& mesh)
		:
		layout(std::move(layout_in))
	{
		Resize(mesh.mNumVertices);

		// A temporary structure to hold bone data per-vertex
		struct VertexBoneData
		{
			DirectX::XMINT4 ids = { -1, -1, -1, -1 }; // Use -1 as uninitialized
			DirectX::XMFLOAT4 weights = { 0.0f, 0.0f, 0.0f, 0.0f };

			void Add(UINT boneID, float weight)
			{
				// Find the first available slot
				int* id_array[] = { &ids.x, &ids.y, &ids.z, &ids.w };
				float* weight_array[] = { &weights.x, &weights.y, &weights.z, &weights.w };
				for (int i = 0; i < 4; i++)
				{
					if (*id_array[i] == -1)
					{
						*id_array[i] = boneID;
						*weight_array[i] = weight;
						return;
					}
				}
				// Optional: Could implement logic here to replace the lowest-weight bone
				// if more than 4 bones influence a vertex. For now, we take the first 4.
			}
		};

		// Pre-process bone data if the layout requires it
		std::vector<VertexBoneData> boneData;
		const bool hasBones = layout.Has(VertexLayout::ElementType::BoneIDs) || layout.Has(VertexLayout::ElementType::BoneWeights);
		if (hasBones)
		{
			boneData.resize(mesh.mNumVertices);
			for (UINT boneIndex = 0; boneIndex < mesh.mNumBones; ++boneIndex)
			{
				const auto& bone = *mesh.mBones[boneIndex];
				for (UINT weightIndex = 0; weightIndex < bone.mNumWeights; ++weightIndex)
				{
					const auto& vertexWeight = bone.mWeights[weightIndex];
					boneData[vertexWeight.mVertexId].Add(boneIndex, vertexWeight.mWeight);
				}
			}
		}

		// Iterate through all vertices and fill the buffer element by element
		for (size_t i = 0; i < mesh.mNumVertices; i++)
		{
			// Get a proxy to the current vertex in our buffer
			auto v = (*this)[i];
			// Iterate through the elements defined in the layout
			for (const auto& e : layout.elements)
			{
				switch (e.GetType())
				{
				case VertexLayout::ElementType::Position3D:
					v.Attr<VertexLayout::ElementType::Position3D>() = *reinterpret_cast<const DirectX::XMFLOAT3*>(&mesh.mVertices[i]);
					break;
				case VertexLayout::ElementType::Normal:
					v.Attr<VertexLayout::ElementType::Normal>() = *reinterpret_cast<const DirectX::XMFLOAT3*>(&mesh.mNormals[i]);
					break;
				case VertexLayout::ElementType::Texture2D:
					v.Attr<VertexLayout::ElementType::Texture2D>() = *reinterpret_cast<const DirectX::XMFLOAT2*>(&mesh.mTextureCoords[0][i]);
					break;
				case VertexLayout::ElementType::Tangent:
					v.Attr<VertexLayout::ElementType::Tangent>() = *reinterpret_cast<const DirectX::XMFLOAT3*>(&mesh.mTangents[i]);
					break;
				case VertexLayout::ElementType::Bitangent:
					v.Attr<VertexLayout::ElementType::Bitangent>() = *reinterpret_cast<const DirectX::XMFLOAT3*>(&mesh.mBitangents[i]);
					break;
				case VertexLayout::ElementType::Float3Color:
					v.Attr<VertexLayout::ElementType::Float3Color>() = *reinterpret_cast<const DirectX::XMFLOAT3*>(&mesh.mColors[0][i]);
					break;
				case VertexLayout::ElementType::Float4Color:
					v.Attr<VertexLayout::ElementType::Float4Color>() = *reinterpret_cast<const DirectX::XMFLOAT4*>(&mesh.mColors[0][i]);
					break;
				case VertexLayout::ElementType::BGRAColor:
					v.Attr<VertexLayout::ElementType::BGRAColor>() = *reinterpret_cast<const BGRAColor*>(&mesh.mColors[0][i]);
					break;
				case VertexLayout::ElementType::BoneIDs:
					v.Attr<VertexLayout::ElementType::BoneIDs>() = boneData[i].ids;
					break;
				case VertexLayout::ElementType::BoneWeights:
					// It's a good practice to normalize the weights, though Assimp often provides normalized data.
					// This is a simplified normalization; a robust version would handle a sum of 0.
					const auto& w = boneData[i].weights;
					float totalWeight = w.x + w.y + w.z + w.w;
					if (totalWeight > 0.0f) {
						v.Attr<VertexLayout::ElementType::BoneWeights>() = { w.x / totalWeight, w.y / totalWeight, w.z / totalWeight, w.w / totalWeight };
					}
					else {
						v.Attr<VertexLayout::ElementType::BoneWeights>() = { 0.0f, 0.0f, 0.0f, 0.0f };
					}
					break;
					// Add other cases as needed
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
		return buffer.size() / layout.Size();
	}
	size_t VertexBuffer::SizeBytes() const noxnd
	{
		return buffer.size();
	}
	Vertex VertexBuffer::Back() noxnd
	{
		assert(buffer.size() != 0u);
		return Vertex{ buffer.data() + buffer.size() - layout.Size(),layout };
	}
	Vertex VertexBuffer::Front() noxnd
	{
		assert(buffer.size() != 0u);
		return Vertex{ buffer.data(),layout };
	}
	Vertex VertexBuffer::operator[](size_t i) noxnd
	{
		assert(i < Size());
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