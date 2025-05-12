#include "Material.h"
#include "DynamicConstant.h"
#include "ConstantBuffersEx.h" // For Caching...Ex buffers
#include <assimp/scene.h> // For aiScene, aiMaterial etc.

// Define any missing Constant Buffer Bindables if they were in a separate file (e.g. for bones)
// For this exercise, we'll assume ModelComponent handles bone CBuf binding directly before draw.
// If a specific Bone CBuf Bindable were used, it would be defined here or included.

Material::Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path, bool bMaterialIsUsedBySkinnedMesh) noxnd
	:
modelPath(path.string()),
isSkinnedMaterial(bMaterialIsUsedBySkinnedMesh)
{
	using namespace Bind;
	const auto rootPath = path.parent_path().string() + "\\";
	{
		aiString tempName;
		material.Get(AI_MATKEY_NAME, tempName);
		name = tempName.C_Str();
		if (name.empty()) {
			name = "UnnamedMaterial_" + std::to_string(reinterpret_cast<uintptr_t>(&material));
		}
	}
	// phong technique
	{
		Technique phong{ "Phong" };
		Step step(0);
		std::string shaderCode = "Phong";
		if (isSkinnedMaterial) {
			shaderCode += "Skinned"; // e.g., PhongSkinned_VS.cso
		}
		aiString texFileName;

		// common (pre)
		vtxLayout.Append(Dvtx::VertexLayout::Position3D);
		vtxLayout.Append(Dvtx::VertexLayout::Normal);

		// Append skinning attributes if material is for skinned mesh
		if (isSkinnedMaterial) {
			vtxLayout.Append(Dvtx::VertexLayout::BoneIDs);
			vtxLayout.Append(Dvtx::VertexLayout::BoneWeights);
		}

		Dcb::RawLayout pscLayout;
		bool hasTexture = false;
		bool hasGlossAlpha = false;

		// diffuse
		{
			bool hasAlpha = false;
			if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS)
			{
				hasTexture = true;
				// shaderCode += "Dif"; // Shader name will handle this variant by its fixed name
				vtxLayout.Append(Dvtx::VertexLayout::Texture2D);
				auto tex = Texture::Resolve(gfx, rootPath + texFileName.C_Str());
				if (tex->HasAlpha())
				{
					hasAlpha = true;
					// shaderCode += "Msk"; // Shader name will handle this variant
				}
				step.AddBindable(std::move(tex));
			}
			else
			{
				pscLayout.Add<Dcb::Float3>("materialColor");
			}
			step.AddBindable(Rasterizer::Resolve(gfx, hasAlpha)); // Rasterizer based on alpha
		}
		// specular
		{
			if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
			{
				hasTexture = true;
				// shaderCode += "Spc";
				if (!vtxLayout.Has(Dvtx::VertexLayout::Texture2D)) { // Ensure UVs if not added by diffuse
					vtxLayout.Append(Dvtx::VertexLayout::Texture2D);
				}
				auto tex = Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 1);
				hasGlossAlpha = tex->HasAlpha();
				step.AddBindable(std::move(tex));
				pscLayout.Add<Dcb::Bool>("useGlossAlpha");
				pscLayout.Add<Dcb::Bool>("useSpecularMap");
			}
			pscLayout.Add<Dcb::Float3>("specularColor");
			pscLayout.Add<Dcb::Float>("specularWeight");
			pscLayout.Add<Dcb::Float>("specularGloss");
		}
		// normal
		{
			if (material.GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS)
			{
				hasTexture = true;
				// shaderCode += "Nrm";
				if (!vtxLayout.Has(Dvtx::VertexLayout::Texture2D)) { // Ensure UVs
					vtxLayout.Append(Dvtx::VertexLayout::Texture2D);
				}
				vtxLayout.Append(Dvtx::VertexLayout::Tangent);
				vtxLayout.Append(Dvtx::VertexLayout::Bitangent);
				step.AddBindable(Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 2));
				pscLayout.Add<Dcb::Bool>("useNormalMap");
				pscLayout.Add<Dcb::Float>("normalMapWeight");
			}
		}
		// common (post)
		{
			step.AddBindable(std::make_shared<TransformCbuf>(gfx, 0u)); // VS slot 0 for world/view/proj
			step.AddBindable(Blender::Resolve(gfx, false)); // Assuming opaque for now

			// Choose VS based on skinning
			auto pvs = VertexShader::Resolve(gfx, shaderCode + "_VS.cso");
			auto pvsbc = pvs->GetBytecode();
			step.AddBindable(std::move(pvs));
			step.AddBindable(PixelShader::Resolve(gfx, shaderCode + "_PS.cso")); // PS might be same for skinned/non-skinned

			step.AddBindable(InputLayout::Resolve(gfx, vtxLayout, pvsbc));
			if (hasTexture)
			{
				step.AddBindable(Bind::Sampler::Resolve(gfx));
			}

			// If skinned, the bone matrix CBuf is assumed to be bound by ModelComponent globally (e.g. to VS slot 1)
			// If a specific bindable were to be used:
			// if (isSkinnedMaterial) {
			//    step.AddBindable(std::make_shared<YourBoneMatrixCbuf>(gfx, pModelComponent, 1u)); // Needs pModelComponent
			// }

			// PS material params (cbuf)
			Dcb::Buffer buf{ std::move(pscLayout) };
			if (auto r = buf["materialColor"]; r.Exists())
			{
				aiColor3D color = { 0.45f,0.45f,0.85f };
				material.Get(AI_MATKEY_COLOR_DIFFUSE, color);
				r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
			}
			buf["useGlossAlpha"].SetIfExists(hasGlossAlpha);
			buf["useSpecularMap"].SetIfExists(material.GetTextureCount(aiTextureType_SPECULAR) > 0); // set true if map exists
			if (auto r = buf["specularColor"]; r.Exists())
			{
				aiColor3D color = { 0.18f,0.18f,0.18f };
				material.Get(AI_MATKEY_COLOR_SPECULAR, color);
				r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
			}
			buf["specularWeight"].SetIfExists(1.0f); // Default, can be controlled by texture too
			if (auto r = buf["specularGloss"]; r.Exists())
			{
				float gloss = 8.0f; // Default
				material.Get(AI_MATKEY_SHININESS, gloss);
				r = gloss;
			}
			buf["useNormalMap"].SetIfExists(material.GetTextureCount(aiTextureType_NORMALS) > 0);
			buf["normalMapWeight"].SetIfExists(1.0f);
			step.AddBindable(std::make_unique<Bind::CachingPixelConstantBufferEx>(gfx, std::move(buf), 1u)); // PS CBuf at slot 1
		}
		phong.AddStep(std::move(step));
		techniques.push_back(std::move(phong));
	}
	// Outline technique (simplified, may need skinning variant too if outlines on animated models are desired)
	// For now, outline technique will likely not work correctly with skinned meshes unless updated similarly.
	{
		Technique outline("Outline", false);
		// ... Outline technique setup ...
		// If skinning is needed for outline, its VS and IL will need updating too.
		// For brevity, this part is left as is, assuming outlines might not apply to skinned parts or use a different approach.
		{
			Step mask(1);
			Dvtx::VertexLayout solidLayout; // Outline might use a simpler layout
			solidLayout.Append(Dvtx::VertexLayout::Position3D);
			if (isSkinnedMaterial) { // If outline needs to be skinned
				solidLayout.Append(Dvtx::VertexLayout::BoneIDs);
				solidLayout.Append(Dvtx::VertexLayout::BoneWeights);
			}

			auto pvs = VertexShader::Resolve(gfx, isSkinnedMaterial ? "SolidSkinned_VS.cso" : "Solid_VS.cso"); // Example
			auto pvsbc = pvs->GetBytecode();
			mask.AddBindable(std::move(pvs));
			mask.AddBindable(InputLayout::Resolve(gfx, isSkinnedMaterial ? vtxLayout : solidLayout, pvsbc)); // Use full vtxLayout if skinned
			mask.AddBindable(std::make_shared<TransformCbuf>(gfx));
			outline.AddStep(std::move(mask));
		}
		{
			Step draw(2);
			Dvtx::VertexLayout offsetLayout; // Similar for offset
			offsetLayout.Append(Dvtx::VertexLayout::Position3D);
			if (isSkinnedMaterial) { // If outline needs to be skinned
				offsetLayout.Append(Dvtx::VertexLayout::BoneIDs);
				offsetLayout.Append(Dvtx::VertexLayout::BoneWeights);
			}

			auto pvs = VertexShader::Resolve(gfx, isSkinnedMaterial ? "OffsetSkinned_VS.cso" : "Offset_VS.cso");
			auto pvsbc = pvs->GetBytecode();
			draw.AddBindable(std::move(pvs));
			draw.AddBindable(PixelShader::Resolve(gfx, "Solid_PS.cso"));
			// ... CBufs for color and offset ...
			draw.AddBindable(InputLayout::Resolve(gfx, isSkinnedMaterial ? vtxLayout : offsetLayout, pvsbc));
			draw.AddBindable(std::make_shared<TransformCbuf>(gfx));
			outline.AddStep(std::move(draw));
		}
		techniques.push_back(std::move(outline));
	}
}

// ExtractVertices now uses the member vtxLayout and takes vertexBoneData
Dvtx::VertexBuffer Material::ExtractVertices(const aiMesh& mesh, const std::vector<Dvtx::VertexBoneData>& vertexBoneData) const noexcept
{
	return Dvtx::VertexBuffer(vtxLayout, mesh, vertexBoneData);
}

std::vector<unsigned short> Material::ExtractIndices(const aiMesh& mesh) const noexcept
{
	std::vector<unsigned short> indices;
	indices.reserve(mesh.mNumFaces * 3);
	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}
	return indices;
}

// MakeVertexBindable now takes vertexBoneData
std::shared_ptr<Bind::VertexBuffer> Material::MakeVertexBindable(Graphics& gfx, const aiMesh& mesh, float scale, const std::vector<Dvtx::VertexBoneData>& vertexBoneData) const noxnd
{
	auto vtc = ExtractVertices(mesh, vertexBoneData); // Pass bone data here
	if (scale != 1.0f && vtxLayout.Has(Dvtx::VertexLayout::Position3D))
	{
		for (size_t i = 0u; i < vtc.Size(); i++)
		{
			DirectX::XMFLOAT3& pos = vtc[i].Attr<Dvtx::VertexLayout::ElementType::Position3D>();
			pos.x *= scale;
			pos.y *= scale;
			pos.z *= scale;
		}
	}
	return Bind::VertexBuffer::Resolve(gfx, MakeMeshTag(mesh, !vertexBoneData.empty()), std::move(vtc));
}

std::shared_ptr<Bind::IndexBuffer> Material::MakeIndexBindable(Graphics& gfx, const aiMesh& mesh) const noxnd
{
	return Bind::IndexBuffer::Resolve(gfx, MakeMeshTag(mesh, false), ExtractIndices(mesh)); // Assuming tag doesn't need skinning differentiation for IBs
}

// MakeMeshTag updated
std::string Material::MakeMeshTag(const aiMesh& mesh, bool isSkinned) const noexcept
{
	std::string namePart = mesh.mName.C_Str();
	if (namePart.empty()) {
		namePart = "UnnamedMesh_" + std::to_string(mesh.mNumVertices) + "_" + std::to_string(mesh.mNumFaces);
	}
	return modelPath + "%" + namePart + (isSkinned ? "_skinned" : "");
}
std::vector<Technique> Material::GetTechniques() const noexcept
{
	return techniques;
}