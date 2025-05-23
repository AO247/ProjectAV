#define DVTX_SOURCE_FILE
#include "Vertex.h"

namespace Dvtx
{
	// VertexLayout
	const VertexLayout::Element& VertexLayout::ResolveByIndex( size_t i ) const noxnd
	{
		return elements[i];
	}
	VertexLayout& VertexLayout::Append( ElementType type ) noxnd
	{
		if( !Has( type ) )
		{
			elements.emplace_back( type,Size() );
		}
		return *this;
	}
	bool VertexLayout::Has( ElementType type ) const noexcept
	{
		for( auto& e : elements )
		{
			if( e.GetType() == type )
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
		desc.reserve( GetElementCount() );
		for( const auto& e : elements )
		{
			desc.push_back( e.GetDesc() );
		}
		return desc;
	}
	std::string VertexLayout::GetCode() const noxnd
	{
		std::string code;
		for( const auto& e : elements )
		{
			code += e.GetCode();
		}
		return code;
	}


	// VertexLayout::Element
	VertexLayout::Element::Element( ElementType type,size_t offset )
		:
		type( type ),
		offset( offset )
	{}
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
		return SizeOf( type );
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
			return sizeof( VertexLayout::Map<type>::SysType );
		}
	};
	constexpr size_t VertexLayout::Element::SizeOf( ElementType type ) noxnd
	{
		return Bridge<SysSizeLookup>( type );
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
		return Bridge<CodeLookup>( type );
	}

	template<VertexLayout::ElementType type> struct DescGenerate {
		static constexpr D3D11_INPUT_ELEMENT_DESC Exec( size_t offset ) noexcept {
			return { 
				VertexLayout::Map<type>::semantic,0,
				VertexLayout::Map<type>::dxgiFormat,
				0,(UINT)offset,D3D11_INPUT_PER_VERTEX_DATA,0
			};
		}
	};
	D3D11_INPUT_ELEMENT_DESC VertexLayout::Element::GetDesc() const noxnd
	{
		return Bridge<DescGenerate>( type,GetOffset() );
	}


	// Vertex
	Vertex::Vertex( char* pData,const VertexLayout& layout ) noxnd
		:
		pData( pData ),
		layout( layout )
	{
		assert( pData != nullptr );
	}
	ConstVertex::ConstVertex( const Vertex& v ) noxnd
		:
		vertex( v )
	{}


	// VertexBuffer
	VertexBuffer::VertexBuffer( VertexLayout layout,size_t size ) noxnd
		:
		layout( std::move( layout ) )
	{
		Resize( size );
	}
	void VertexBuffer::Resize( size_t newSize ) noxnd
	{
		const auto size = Size();
		if( size < newSize )
		{
			buffer.resize( buffer.size() + layout.Size() * (newSize - size) );
		}
	}
	const char* VertexBuffer::GetData() const noxnd
	{
		return buffer.data();
	}

	template<VertexLayout::ElementType type>
	struct AttributeAiMeshFill
	{
		static constexpr void Exec(VertexBuffer* pBuf, const aiMesh& mesh) noxnd
		{
			// Add a condition to skip bone types
			if constexpr (type == VertexLayout::BoneIDs || type == VertexLayout::BoneWeights)
			{
				// Do nothing for bone types here; they will be filled manually later.
				// You could optionally zero out the memory for these attributes here if desired,
				// though ModelComponent will overwrite it.
				if constexpr (type == VertexLayout::BoneIDs) {
					for (auto end = mesh.mNumVertices, i = 0u; i < end; i++) {
						auto& ids = (*pBuf)[i].Attr<VertexLayout::BoneIDs>();
						for (int j = 0; j < MAX_BONES_PER_VERTEX; ++j) ids[j] = 0; // Default init
					}
				}
				else if constexpr (type == VertexLayout::BoneWeights) {
					for (auto end = mesh.mNumVertices, i = 0u; i < end; i++) {
						auto& weights = (*pBuf)[i].Attr<VertexLayout::BoneWeights>();
						for (int j = 0; j < MAX_BONES_PER_VERTEX; ++j) weights[j] = 0.0f; // Default init
						// Optionally, set weights[0] = 1.0f if no bones assigned yet for static meshes
						if (mesh.mNumBones == 0 && MAX_BONES_PER_VERTEX > 0) {
							weights[0] = 1.0f;
						}
					}
				}
				return;
			}
			else // For all other types, proceed as before
			{
				// Check if the source mesh actually has the data for non-bone types
				// This requires a way to check if, e.g., mesh.mNormals is non-null
				// The Map<type>::Extract macro doesn't inherently do this check.
				// For example:
				if constexpr (type == VertexLayout::Normal) {
					if (!mesh.HasNormals()) return;
				}
				else if constexpr (type == VertexLayout::Texture2D) {
					if (!mesh.HasTextureCoords(0)) return;
				}
				else if constexpr (type == VertexLayout::Tangent) {
					if (!mesh.HasTangentsAndBitangents()) return; // Tangents often imply Bitangents
				}
				else if constexpr (type == VertexLayout::Bitangent) {
					if (!mesh.HasTangentsAndBitangents()) return;
				} // Add similar checks for mColors if you use them

				for (auto end = mesh.mNumVertices, i = 0u; i < end; i++)
				{
					(*pBuf)[i].Attr<type>() = VertexLayout::Map<type>::Extract(mesh, i);
				}
			}
		}
	};

	VertexBuffer::VertexBuffer( VertexLayout layout_in,const aiMesh& mesh )
		:
		layout( std::move( layout_in ) )
	{
		Resize( mesh.mNumVertices );
		for( size_t i = 0,end = layout.GetElementCount(); i < end; i++ )
		{
			VertexLayout::Bridge<AttributeAiMeshFill>( layout.ResolveByIndex( i ).GetType(),this,mesh );
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
		assert( buffer.size() != 0u );
		return Vertex{ buffer.data() + buffer.size() - layout.Size(),layout };
	}
	Vertex VertexBuffer::Front() noxnd
	{
		assert( buffer.size() != 0u );
		return Vertex{ buffer.data(),layout };
	}
	Vertex VertexBuffer::operator[]( size_t i ) noxnd
	{
		assert( i < Size() );
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
	ConstVertex VertexBuffer::operator[]( size_t i ) const noxnd
	{
		return const_cast<VertexBuffer&>(*this)[i];
	}
}