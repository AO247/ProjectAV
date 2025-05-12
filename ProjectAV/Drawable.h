#pragma once
#include "Graphics.h"
#include <DirectXMath.h>
#include "ConditionalNoexcept.h"
#include <memory>
#include "Technique.h"
#include "Vertex.h" // Include Vertex.h for Dvtx::VertexBoneData

class TechniqueProbe;
class Material;
struct aiMesh;

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
	// Modified constructor to accept vertexBoneData
	Drawable(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale = 1.0f,
		const std::vector<Dvtx::VertexBoneData>& vertexBoneData = {}) noexcept;
	Drawable(const Drawable&) = delete;
	void AddTechnique(Technique tech_in) noexcept;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Submit(class FrameCommander& frame) const noexcept;
	void Bind(Graphics& gfx) const noexcept;
	void Accept(TechniqueProbe& probe);
	UINT GetIndexCount() const noxnd;
	virtual ~Drawable();
protected:
	std::shared_ptr<Bind::IndexBuffer> pIndices;
	std::shared_ptr<Bind::VertexBuffer> pVertices;
	std::shared_ptr<Bind::Topology> pTopology;
	std::vector<Technique> techniques;
};