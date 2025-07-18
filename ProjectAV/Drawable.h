#pragma once
#include "Graphics.h"
#include <DirectXMath.h>
#include "ConditionalNoexcept.h"
#include <memory>
#include "Technique.h"
#include "Bindable.h"

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

namespace Rgph
{
	class RenderGraph;
}

class Drawable
{
public:
	Drawable() = default;
	Drawable(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale = 1.0f) noexcept;
	Drawable(const Drawable&) = delete;
	void AddTechnique(Technique tech_in) noexcept;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Submit() const noexcept;
	void Bind(Graphics& gfx) const noxnd;
	void Accept(TechniqueProbe& probe);
	UINT GetIndexCount() const noxnd;
	virtual void LinkTechniques(Rgph::RenderGraph&);
	virtual ~Drawable();
	virtual const std::vector<DirectX::XMMATRIX>* GetBoneTransformsPtr() const noexcept;

	std::shared_ptr<const Bind::VertexBuffer> GetVertexBuffer() const { return pVertices; }
	std::shared_ptr<const Bind::IndexBuffer> GetIndexBuffer() const { return pIndices; }

protected:
	std::shared_ptr<Bind::IndexBuffer> pIndices;	
	std::shared_ptr<Bind::VertexBuffer> pVertices;
	std::shared_ptr<Bind::Topology> pTopology;
	std::vector<Technique> techniques;
};