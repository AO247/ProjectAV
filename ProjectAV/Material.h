// In Material.h
#pragma once
#include "Graphics.h"
#include "BindableCommon.h" // For Bind namespace forward declarations if not directly included
#include <vector>
#include <filesystem>
#include <memory>           // For std::shared_ptr
#include "Technique.h"
#include "Vertex.h"         // For Dvtx::VertexBuffer and Dvtx::VertexLayout

struct aiMaterial;
struct aiMesh;

// Forward declare Bindables (if not included via BindableCommon.h or similar)
namespace Bind {
    class VertexBuffer;
    class IndexBuffer;
}

// **** NEW RETURN STRUCTURES ****
struct VertexDataBundle
{
    std::shared_ptr<Bind::VertexBuffer> gpuBuffer;
    Dvtx::VertexBuffer cpuBuffer; // This will hold the scaled vertex data
};

struct IndexDataBundle
{
    std::shared_ptr<Bind::IndexBuffer> gpuBuffer;
    std::vector<unsigned short> cpuIndices;
};


class Material
{
public:
    Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path) noxnd;
    // ExtractVertices and ExtractIndices can remain as they are, used internally by Make...Bindable
    Dvtx::VertexBuffer ExtractVertices(const aiMesh& mesh) const noexcept;
    std::vector<unsigned short> ExtractIndices(const aiMesh& mesh) const noexcept;

    // **** MODIFY RETURN TYPES OF Make...Bindable ****
    VertexDataBundle MakeVertexDataBundle(Graphics& gfx, const aiMesh& mesh, float scale = 1.0f) const noxnd;
    IndexDataBundle MakeIndexDataBundle(Graphics& gfx, const aiMesh& mesh) const noxnd;

    std::vector<Technique> GetTechniques() const noexcept;
private:
    std::string MakeMeshTag(const aiMesh& mesh) const noexcept;
private:
    Dvtx::VertexLayout vtxLayout; // This is good, used by ExtractVertices
    std::vector<Technique> techniques;
    std::string modelPath;
    std::string name;
};