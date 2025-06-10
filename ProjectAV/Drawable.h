// Drawable.h

#pragma once
#include "Graphics.h"
#include <DirectXMath.h>
#include "ConditionalNoexcept.h"
#include <memory>
#include "Technique.h"
// It's good practice to forward declare Bind::VertexBuffer if only its pointer/reference type is used in the header
// However, if you return std::shared_ptr<Bind::VertexBuffer>, you'll need the full definition or a specific forward declaration for shared_ptr.
// Let's assume Bindable.h or a similar header gives the full definition of Bind::VertexBuffer
#include "Bindable.h" // Or the specific header for Bind::VertexBuffer

class TechniqueProbe;
class Material;
struct aiMesh;

// Namespace Bind might not be fully declared yet if Bindable.h isn't included,
// but std::shared_ptr<Bind::VertexBuffer> requires Bind::VertexBuffer to be a complete type
// or for std::shared_ptr to be specialized for an incomplete type (which is advanced).
// Simplest is to ensure Bind::VertexBuffer is defined before this point.

namespace Bind
{
	class IndexBuffer;
	class VertexBuffer; // Forward declaration
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
	void LinkTechniques(Rgph::RenderGraph&);
	virtual ~Drawable();

	// **** ADD THIS PUBLIC GETTER ****
	// It should return a const shared_ptr if the intent is read-only access
	// and the Bind::VertexBuffer methods you call on it are const.
	// If Bind::VertexBuffer methods are non-const but you're only reading,
	// it's fine, but conceptually a const shared_ptr to a const object is cleaner.
	// For now, let's assume Bind::VertexBuffer methods (like GetLayout, Size, operator[]) are const.
	std::shared_ptr<const Bind::VertexBuffer> GetVertexBuffer() const { return pVertices; }
	std::shared_ptr<const Bind::IndexBuffer> GetIndexBuffer() const { return pIndices; }
	// If Bind::VertexBuffer methods are NOT const, or you need non-const access elsewhere (unlikely for this getter's purpose)
	// you might use: std::shared_ptr<Bind::VertexBuffer> GetVertexBuffer() const { return pVertices; }
	// The `const` on the method GetVertexBuffer() means this method can be called on const Drawable objects.
	// The `const` in std::shared_ptr<const Bind::VertexBuffer> means the pointed-to VertexBuffer cannot be modified through this shared_ptr.

protected:
	std::shared_ptr<Bind::IndexBuffer> pIndices;	
	std::shared_ptr<Bind::VertexBuffer> pVertices;
	std::shared_ptr<Bind::Topology> pTopology;
	std::vector<Technique> techniques;
};