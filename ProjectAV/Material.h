#pragma once
#include "Vertex.h"
#include "Graphics.h"
#include "BindableCommon.h"
#include <vector>
#include <filesystem>
#include "Technique.h"
 // Required for Dvtx::VertexBoneData

struct aiMaterial;
struct aiMesh;
struct aiScene; // Forward declare aiScene

class Material
{
public:
	// Constructor now takes a hint if this material might be used by a skinned mesh
	Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path, bool bMaterialIsUsedBySkinnedMesh) noxnd;

	// ExtractVertices now takes pre-processed bone data
	Dvtx::VertexBuffer ExtractVertices(const aiMesh& mesh, const std::vector<Dvtx::VertexBoneData>& vertexBoneData) const noexcept;
	std::vector<unsigned short> ExtractIndices(const aiMesh& mesh) const noexcept;

	// MakeVertexBindable now takes pre-processed bone data
	std::shared_ptr<Bind::VertexBuffer> MakeVertexBindable(Graphics& gfx, const aiMesh& mesh, float scale, const std::vector<Dvtx::VertexBoneData>& vertexBoneData) const noxnd;
	std::shared_ptr<Bind::IndexBuffer> MakeIndexBindable(Graphics& gfx, const aiMesh& mesh) const noxnd;
	std::vector<Technique> GetTechniques() const noexcept;
	const Dvtx::VertexLayout& GetVertexLayout() const noexcept { return vtxLayout; } // Added getter

private:
	std::string MakeMeshTag(const aiMesh& mesh, bool isSkinned) const noexcept; // Added isSkinned flag
private:
	Dvtx::VertexLayout vtxLayout;
	std::vector<Technique> techniques;
	std::string modelPath;
	std::string name;
	bool isSkinnedMaterial = false; // Store if this material is configured for skinning
};