#pragma once
#include "Graphics.h"
#include <DirectXMath.h>
#include "ConditionalNoexcept.h"
#include <memory>
#include "Technique.h"

class TechniqueProbe;
class Material;
struct aiMesh;
class Node;

namespace Bind
{
	class IndexBuffer;
	class VertexBuffer;
	class Topology;
	class InputLayout;
}

class Drawable
{
public:
	Drawable() = default;
	Drawable(
		Node* pOwnerNode,
		Graphics& gfx, // For pTopology
		std::shared_ptr<Bind::VertexBuffer> pVB,
		std::shared_ptr<Bind::IndexBuffer> pIB,
		const Material& mat // For techniques
	) noexcept;	
	Drawable( const Drawable& ) = delete;
	void AddTechnique( Technique tech_in ) noexcept;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Submit( class FrameCommander& frame ) const noexcept;
	void Bind( Graphics& gfx ) const noexcept;
	void Accept( TechniqueProbe& probe );
	UINT GetIndexCount() const noxnd;
	virtual ~Drawable();

	Node* GetParentNode() const noexcept;  



protected:
	std::shared_ptr<Bind::IndexBuffer> pIndices;
	std::shared_ptr<Bind::VertexBuffer> pVertices;
	std::shared_ptr<Bind::Topology> pTopology;
	std::vector<Technique> techniques;

	Node* pOwnerNode = nullptr;
};