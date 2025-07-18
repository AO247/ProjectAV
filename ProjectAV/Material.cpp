#include "Material.h"
#include "BindableCommon.h"
#include "DynamicConstant.h"
#include "ConstantBuffersEx.h"
#include "SkinningCBuf.h"
#include "TransformCbufScaling.h"
#include "Stencil.h"
#include <filesystem>

Material::Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path, bool isSkinned) noxnd
	:
modelPath(path.string())
{
	using namespace Bind;
	const auto rootPath = path.parent_path().string() + "\\";
	{
		aiString tempName;
		material.Get(AI_MATKEY_NAME, tempName);
		name = tempName.C_Str();
	}
	std::string materialNameForLog = name;
	if (materialNameForLog.empty()) {
		materialNameForLog = "[Unnamed Assimp Material]";
	} 

	// =======================================================================
	// CONDITIONAL LOGIC FOR SKINNED VS. STATIC MODELS
	// =======================================================================
	if (isSkinned)
	{ 
		Technique skinnedTech{ "SkinnedTextured", true };
		Step step("lambertian");

		vtxLayout.Append(Dvtx::VertexLayout::Position3D);
		vtxLayout.Append(Dvtx::VertexLayout::Normal);
		vtxLayout.Append(Dvtx::VertexLayout::Texture2D);
		vtxLayout.Append(Dvtx::VertexLayout::Tangent);
		vtxLayout.Append(Dvtx::VertexLayout::Bitangent);
		vtxLayout.Append(Dvtx::VertexLayout::BoneIDs);
		vtxLayout.Append(Dvtx::VertexLayout::BoneWeights);

		auto pvs = VertexShader::Resolve(gfx, "Skinned_VS.cso");
		step.AddBindable(InputLayout::Resolve(gfx, vtxLayout, *pvs));
		step.AddBindable(std::move(pvs));
		step.AddBindable(PixelShader::Resolve(gfx, "Skinned_PS.cso"));

		Dcb::RawLayout pscLayout;
		aiString texFileName;
		bool hasDiffuseTextureLoaded = false;
		 
		pscLayout.Add<Dcb::Float3>("materialColor");    
		pscLayout.Add<Dcb::Bool>("hasDiffuseMap");      

		pscLayout.Add<Dcb::Float3>("specularColor");
		pscLayout.Add<Dcb::Bool>("useNormalMap");       

		pscLayout.Add<Dcb::Float>("normalMapWeight");
		pscLayout.Add<Dcb::Float>("specularWeight");
		pscLayout.Add<Dcb::Float>("specularGloss");
		 
		if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS)
		{
			step.AddBindable(Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 0u)); // Diffuse on t0
			hasDiffuseTextureLoaded = true;
		}

		// Try to load normal map
		bool hasNormalMapLoaded = false;
		if (material.GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS)
		{
			step.AddBindable(Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 2u)); // Normal map on t2
			hasNormalMapLoaded = true;
		}

		step.AddBindable(Sampler::Resolve(gfx));

		// Populate Constant Buffer
		Dcb::Buffer buf{ std::move(pscLayout) };
		 
		if (hasDiffuseTextureLoaded) {
			buf["materialColor"] = DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f }; // Placeholder when texture is used
		}
		else {
 

			aiColor3D diffuseColorValue = { 0.9f, 0.75f, 0.65f }; 
			buf["materialColor"] = reinterpret_cast<DirectX::XMFLOAT3&>(diffuseColorValue);
		}
		buf["hasDiffuseMap"] = hasDiffuseTextureLoaded;

		aiColor3D specularColorValue = { 1.0f, 1.0f, 1.0f };
		material.Get(AI_MATKEY_COLOR_SPECULAR, specularColorValue);
		buf["specularColor"] = reinterpret_cast<DirectX::XMFLOAT3&>(specularColorValue);

		buf["useNormalMap"] = hasNormalMapLoaded;
		buf["normalMapWeight"] = 1.0f;

		float specWeightVal = 0.05f;;
		material.Get(AI_MATKEY_SHININESS_STRENGTH, specWeightVal);
		buf["specularWeight"] = specWeightVal;

		float glossVal = 10.0f;
		material.Get(AI_MATKEY_SHININESS, glossVal);
		buf["specularGloss"] = glossVal;

		step.AddBindable(std::make_unique<Bind::CachingPixelConstantBufferEx>(gfx, std::move(buf), 1u)); // Bind to b1

		step.AddBindable(std::make_shared<TransformCbuf>(gfx));
		step.AddBindable(std::make_shared<SkinningCbuf>(gfx, 3u));

		skinnedTech.AddStep(std::move(step));
		techniques.push_back(std::move(skinnedTech));
	}
	else
	{
		// --- STATIC (PHONG) MODEL PATH --- (This is your existing code)
		Technique phong{ "Phong", true };
		Step step("lambertian");
		std::string shaderCode = "Phong";
		aiString texFileName;

		vtxLayout.Append(Dvtx::VertexLayout::Position3D);
		vtxLayout.Append(Dvtx::VertexLayout::Normal);
		Dcb::RawLayout pscLayout;
		bool hasTexture = false;
		bool hasGlossAlpha = false;

		{
			bool hasAlpha = false;
			if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS)
			{
				hasTexture = true;
				shaderCode += "Dif";
				vtxLayout.Append(Dvtx::VertexLayout::Texture2D);
				auto tex = Texture::Resolve(gfx, rootPath + texFileName.C_Str());
				if (tex->HasAlpha())
				{
					hasAlpha = true;
					shaderCode += "Msk";
				}
				step.AddBindable(std::move(tex));
			}
			else
			{
				pscLayout.Add<Dcb::Float3>("materialColor");
			}
			step.AddBindable(Rasterizer::Resolve(gfx, hasAlpha));
		}

		if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
		{
			hasTexture = true;
			shaderCode += "Spc";
			vtxLayout.Append(Dvtx::VertexLayout::Texture2D); // Might be redundant, but safe
			auto tex = Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 1);
			hasGlossAlpha = tex->HasAlpha();
			step.AddBindable(std::move(tex));
			pscLayout.Add<Dcb::Bool>("useGlossAlpha");
			pscLayout.Add<Dcb::Bool>("useSpecularMap");
		}
		pscLayout.Add<Dcb::Float3>("specularColor");
		pscLayout.Add<Dcb::Float>("specularWeight");
		pscLayout.Add<Dcb::Float>("specularGloss");

		if (material.GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS)
		{
			hasTexture = true;
			shaderCode += "Nrm";
			vtxLayout.Append(Dvtx::VertexLayout::Texture2D); // Might be redundant, but safe
			vtxLayout.Append(Dvtx::VertexLayout::Tangent);
			vtxLayout.Append(Dvtx::VertexLayout::Bitangent);
			step.AddBindable(Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 2));
			pscLayout.Add<Dcb::Bool>("useNormalMap");
			pscLayout.Add<Dcb::Float>("normalMapWeight");
		}

		step.AddBindable(std::make_shared<TransformCbuf>(gfx));
		auto pvs = VertexShader::Resolve(gfx, shaderCode + "_VS.cso");
		step.AddBindable(InputLayout::Resolve(gfx, vtxLayout, *pvs));
		step.AddBindable(std::move(pvs));
		step.AddBindable(PixelShader::Resolve(gfx, shaderCode + "_PS.cso"));

		if (hasTexture)
		{
			step.AddBindable(Bind::Sampler::Resolve(gfx));
		}

		Dcb::Buffer buf{ std::move(pscLayout) };
		if (auto r = buf["materialColor"]; r.Exists())
		{
			aiColor3D color = { 0.45f,0.45f,0.85f };
			material.Get(AI_MATKEY_COLOR_DIFFUSE, color);
			r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
		}
		buf["useGlossAlpha"].SetIfExists(hasGlossAlpha);
		buf["useSpecularMap"].SetIfExists(true);
		if (auto r = buf["specularColor"]; r.Exists())
		{
			aiColor3D color = { 0.18f,0.18f,0.18f };
			material.Get(AI_MATKEY_COLOR_SPECULAR, color);
			r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
		}
		buf["specularWeight"].SetIfExists(1.0f);
		if (auto r = buf["specularGloss"]; r.Exists())
		{
			float gloss = 8.0f;
			material.Get(AI_MATKEY_SHININESS, gloss);
			r = gloss;
		}
		buf["useNormalMap"].SetIfExists(true);
		buf["normalMapWeight"].SetIfExists(1.0f);
		step.AddBindable(std::make_unique<Bind::CachingPixelConstantBufferEx>(gfx, std::move(buf), 1u));

		phong.AddStep(std::move(step));
		techniques.push_back(std::move(phong));
	}
	//SHadow technique
	{
		if (isSkinned) {
			OutputDebugStringA(("Name: " + name + "\n").c_str());
			Technique shadow("Shadow");
			Step step("shadow");
			auto pvs = VertexShader::Resolve(gfx, "ShadowSkinned_VS.cso");
			 
			step.AddBindable(InputLayout::Resolve(gfx, vtxLayout, *pvs));
			 
			step.AddBindable(std::move(pvs));
			 
			step.AddBindable(std::make_shared<ShadowCbuf>(gfx));
			 
			step.AddBindable(std::make_shared<SkinningCbuf>(gfx, 3u));  
 
			step.AddBindable(NullPixelShader::Resolve(gfx));

			shadow.AddStep(std::move(step));
			techniques.push_back(std::move(shadow));
		}
		else {
			OutputDebugStringA(("Name: " + name + "\n").c_str());
			Technique shadow("Shadow");
			Step step("shadow");
			auto pvs = VertexShader::Resolve(gfx, "Shadow_VS.cso");

			step.AddBindable(std::make_shared<ShadowCbuf>(gfx));
			step.AddBindable(InputLayout::Resolve(gfx, vtxLayout, *pvs));
			step.AddBindable(std::move(pvs));
			step.AddBindable(NullPixelShader::Resolve(gfx));

			shadow.AddStep(std::move(step));
			techniques.push_back(std::move(shadow));
		}
		
	}
}
Dvtx::VertexBuffer Material::ExtractVertices(const aiMesh& mesh) const noexcept
{
	return { vtxLayout,mesh };
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
std::shared_ptr<Bind::VertexBuffer> Material::MakeVertexBindable(Graphics& gfx, const aiMesh& mesh, float scale) const noxnd
{
	auto vtc = ExtractVertices(mesh);
	if (scale != 1.0f)
	{
		for (auto i = 0u; i < vtc.Size(); i++)
		{
			DirectX::XMFLOAT3& pos = vtc[i].Attr<Dvtx::VertexLayout::ElementType::Position3D>();
			pos.x *= scale;
			pos.y *= scale;
			pos.z *= scale;
		}
	}
	return Bind::VertexBuffer::Resolve(gfx, MakeMeshTag(mesh), std::move(vtc));
}
std::shared_ptr<Bind::IndexBuffer> Material::MakeIndexBindable(Graphics& gfx, const aiMesh& mesh) const noxnd
{
	return Bind::IndexBuffer::Resolve(gfx, MakeMeshTag(mesh), ExtractIndices(mesh));
}
std::string Material::MakeMeshTag(const aiMesh& mesh) const noexcept
{
	return modelPath + "%" + mesh.mName.C_Str();
}
std::vector<Technique> Material::GetTechniques() const noexcept
{
	return techniques;
}