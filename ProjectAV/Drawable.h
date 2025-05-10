// In Drawable.h
#pragma once
#include "Graphics.h"
#include <DirectXMath.h>
#include "ConditionalNoexcept.h"
#include <memory>
#include <vector>
#include "Technique.h"
#include "Vertex.h" // For Dvtx::VertexBuffer and Dvtx::VertexLayout

class TechniqueProbe;
class Material;
struct aiMesh;

namespace Bind
{
    class IndexBuffer;
    class VertexBuffer;
    class Topology;
}

class Drawable
{
public:
    // Provide a working default constructor
    Drawable() noexcept
        : pIndices(nullptr),
        pVertices(nullptr),
        pTopology(nullptr), // Or some default if possible, like Bind::Topology::Resolve(gfx) if gfx were available
        // For a true default, nullptr is safer if Resolve needs gfx.
        // Techniques vector will be default-constructed (empty)
        meshVertexDataCPU(Dvtx::VertexLayout{}, 0u), // Initialize with empty layout and 0 size
        meshIndexDataCPU()                           // Default-construct (empty vector)
    {
    }

    // This constructor is for when Drawable is created from an aiMesh via a Material
    Drawable(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale = 1.0f) noexcept;

    Drawable(const Drawable&) = delete; // Keep copy constructor deleted

    void AddTechnique(Technique tech_in) noexcept;
    virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
    void Submit(class FrameCommander& frame) const noexcept;
    void Bind(Graphics& gfx) const noexcept;
    void Accept(TechniqueProbe& probe);
    UINT GetIndexCount() const noxnd;
    virtual ~Drawable();

    // Getters for CPU data (if you added these as per previous discussions)
    const Dvtx::VertexBuffer& GetVertexDataCPU() const noexcept { return meshVertexDataCPU; }
    const std::vector<unsigned short>& GetIndexDataCPU() const noexcept { return meshIndexDataCPU; }

protected:
    std::shared_ptr<Bind::IndexBuffer> pIndices;
    std::shared_ptr<Bind::VertexBuffer> pVertices;
    std::shared_ptr<Bind::Topology> pTopology;
    std::vector<Technique> techniques;

    Dvtx::VertexBuffer meshVertexDataCPU;
    std::vector<unsigned short> meshIndexDataCPU;
};