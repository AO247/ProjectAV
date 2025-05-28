#include "Material.h"
#include "DynamicConstant.h"
#include "ConstantBuffersEx.h"
#include <assimp/scene.h> // Make sure this is the correct Assimp header
#include "Vertex.h"
#include "BindableCodex.h"
#include "BoneTransformCbuf.h" // Ensure this is included

Material::Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path) noxnd
    : modelPath(path.string()) // modelPath is initialized here
{
    using namespace Bind;
    const auto rootPath = path.parent_path().string() + "\\";
    {
        aiString tempName;
        material.Get(AI_MATKEY_NAME, tempName);
        name = tempName.C_Str(); // Material name from aiMaterial
    }

    // Determine if this material is for a "char" model for special handling
    // We check this->modelPath which stores the full path to the model file.
    bool isCharModel = (this->modelPath.find("char") != std::string::npos);
    // Note: std::string::find returns std::string::npos if substring is not found.
    // This is a simple substring check. You might want a more robust path parsing
    // if "char" could appear in directory names unintentionally.
    // For example, checking the filename part: path.filename().string().find("char")

    // --- Build the Material's base vtxLayout (member: this->vtxLayout) ---
    // This layout will be used by GetRequiredVertexLayout and potentially by the static Phong path.
    this->vtxLayout.Append(Dvtx::VertexLayout::Position3D);
    this->vtxLayout.Append(Dvtx::VertexLayout::Normal);
    // (Conditionally add Texture2D, Tangent, Bitangent based on material properties as before)
    aiString texFileName_forLayout; // temp for layout building
    bool hasDiffuseForLayout = (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName_forLayout) == aiReturn_SUCCESS);
    bool hasNormalsForLayout = (material.GetTexture(aiTextureType_NORMALS, 0, &texFileName_forLayout) == aiReturn_SUCCESS);

    if (hasDiffuseForLayout || isCharModel) { // Add Texture2D if diffuse OR if it's a char model (assuming char model uses at least one texture)
        if (!this->vtxLayout.Has(Dvtx::VertexLayout::Texture2D)) {
            this->vtxLayout.Append(Dvtx::VertexLayout::Texture2D);
        }
    }
    if (hasNormalsForLayout) { // Only add Tangent/Bitangent if a normal map is explicitly found
        if (!this->vtxLayout.Has(Dvtx::VertexLayout::Texture2D)) this->vtxLayout.Append(Dvtx::VertexLayout::Texture2D); // Normals also need UVs
        if (!this->vtxLayout.Has(Dvtx::VertexLayout::Tangent)) this->vtxLayout.Append(Dvtx::VertexLayout::Tangent);
        if (!this->vtxLayout.Has(Dvtx::VertexLayout::Bitangent)) this->vtxLayout.Append(Dvtx::VertexLayout::Bitangent);
    }
    // --- End Base vtxLayout Setup ---


    // --- Main "Phong" Technique ---
    // This technique will now conditionally become a "skinned" technique if isCharModel is true.
    {
        Technique phongOrSkinnedTechnique{ "Phong" }; // Start with "Phong" name
        if (isCharModel) {
            phongOrSkinnedTechnique.SetName("PhongSkinned_char"); // Optionally change name for clarity
        }
        Step step(0);

        std::string vsFilename;
        std::string psFilename;
        Dvtx::VertexLayout currentStepLayout = this->vtxLayout; // Start with the base layout

        bool hasTexture = false; // General texture flag for this technique
        bool hasDiffuseAlpha = false;


        if (isCharModel) {
            vsFilename = "VertexShader.cso"; // Fixed name for char models
            psFilename = "PixelShader.cso"; // Fixed name for char models

            // Ensure layout for char model includes necessary attributes for SkinnedShader_VS
            // Position3D, Normal, Texture2D are likely already in this->vtxLayout
            // Now, add bone attributes specifically for this step's layout
            currentStepLayout.Append(Dvtx::VertexLayout::BoneIDs);
            currentStepLayout.Append(Dvtx::VertexLayout::BoneWeights);

            // Bind the BoneTransformCbuf for char models
            step.AddBindable(BoneTransformCbuf::Resolve(gfx, 3u)); // Assuming slot b3

            // Char models might only use one diffuse texture
            aiString charTexFile;
            if (material.GetTexture(aiTextureType_DIFFUSE, 0, &charTexFile) == aiReturn_SUCCESS) {
                hasTexture = true;
                auto tex = Texture::Resolve(gfx, rootPath + charTexFile.C_Str());
                if (tex->HasAlpha()) {
                    hasDiffuseAlpha = true;
                }
                step.AddBindable(std::move(tex));
            }
        }
        else {
            // --- Original Dynamic Shader Name Logic for non-char models ---
            std::string shaderCodeFeatures = ""; // Build "Dif", "Spc", "Nrm", "Msk" string
            aiString texFileName_phong;
            // Diffuse
            if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName_phong) == aiReturn_SUCCESS) {
                hasTexture = true;
                shaderCodeFeatures += "Dif";
                // UVs should already be in this->vtxLayout if diffuse exists
                auto tex = Texture::Resolve(gfx, rootPath + texFileName_phong.C_Str());
                if (tex->HasAlpha()) {
                    hasDiffuseAlpha = true; // For Rasterizer state
                    shaderCodeFeatures += "Msk";
                }
                step.AddBindable(std::move(tex));
            }
            // Specular
            if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName_phong) == aiReturn_SUCCESS) {
                hasTexture = true;
                shaderCodeFeatures += "Spc";
                // Ensure UVs if not already (this->vtxLayout covers this)
                step.AddBindable(Texture::Resolve(gfx, rootPath + texFileName_phong.C_Str(), 1u));
            }
            // Normal
            if (material.GetTexture(aiTextureType_NORMALS, 0, &texFileName_phong) == aiReturn_SUCCESS) {
                hasTexture = true;
                shaderCodeFeatures += "Nrm";
                // Tangents/Bitangents should already be in this->vtxLayout if normal map exists
                step.AddBindable(Texture::Resolve(gfx, rootPath + texFileName_phong.C_Str(), 2u));
            }
            vsFilename = "Phong" + shaderCodeFeatures + "_VS.cso";
            psFilename = "Phong" + shaderCodeFeatures + "_PS.cso";
            // currentStepLayout is already this->vtxLayout (which is correct for static)
        }

        // Common bindables (pre-shader)
        step.AddBindable(std::make_shared<TransformCbuf>(gfx, 0u)); // b0
        step.AddBindable(Rasterizer::Resolve(gfx, hasDiffuseAlpha)); // Use alpha from diffuse if present
        step.AddBindable(Blender::Resolve(gfx, false));

        // Resolve and bind shaders and InputLayout
        auto pvs = VertexShader::Resolve(gfx, vsFilename);
        auto pvsbc = pvs->GetBytecode();
        step.AddBindable(std::move(pvs));
        step.AddBindable(PixelShader::Resolve(gfx, psFilename));
        // IMPORTANT: InputLayout uses 'currentStepLayout' which is conditional
        step.AddBindable(InputLayout::Resolve(gfx, currentStepLayout, pvsbc));

        if (hasTexture) { // If any texture was bound for this path (char or static)
            step.AddBindable(Bind::Sampler::Resolve(gfx));
        }

        // PS material params (cbuf) - needs to be flexible
        Dcb::RawLayout pscLayout;
        // Populate pscLayout based on what the PS expects (could differ for SkinnedShader_PS vs Phong_PS)
        if (isCharModel) {
            // Simpler PSC for SkinnedShader_PS
            if (!hasTexture) { // If char model uses SkinnedShader_PS but has NO texture from material
                pscLayout.Add<Dcb::Float3>("materialColor");
            }
            pscLayout.Add<Dcb::Float3>("specularColor");
            pscLayout.Add<Dcb::Float>("specularPower"); // Or specularGloss
        }
        else {
            // Original PSC layout for dynamic Phong
            bool hasGlossAlpha_phong = false;
            aiString texFileName_phong_psc; // Separate for clarity
            if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName_phong_psc) != aiReturn_SUCCESS) {
                pscLayout.Add<Dcb::Float3>("materialColor");
            }
            if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName_phong_psc) == aiReturn_SUCCESS) {
                auto specTex = Texture::Resolve(gfx, rootPath + texFileName_phong_psc.C_Str(), 1);
                hasGlossAlpha_phong = specTex->HasAlpha();
                pscLayout.Add<Dcb::Bool>("useGlossAlpha");
                pscLayout.Add<Dcb::Bool>("useSpecularMap");
            }
            pscLayout.Add<Dcb::Float3>("specularColor");
            pscLayout.Add<Dcb::Float>("specularWeight");
            pscLayout.Add<Dcb::Float>("specularGloss");
            if (material.GetTexture(aiTextureType_NORMALS, 0, &texFileName_phong_psc) == aiReturn_SUCCESS) {
                pscLayout.Add<Dcb::Bool>("useNormalMap");
                pscLayout.Add<Dcb::Float>("normalMapWeight");
            }
        }

        Dcb::Buffer buf{ std::move(pscLayout) };
        // Fill buf with aiMaterial values, considering if it's charModel or not
        if (isCharModel) {
            if (auto r = buf["materialColor"]; r.Exists()) {
                aiColor3D color = { 0.8f, 0.8f, 0.8f }; material.Get(AI_MATKEY_COLOR_DIFFUSE, color);
                r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
            }
            if (auto r = buf["specularColor"]; r.Exists()) {
                aiColor3D color = { 0.2f, 0.2f, 0.2f }; material.Get(AI_MATKEY_COLOR_SPECULAR, color);
                r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
            }
            if (auto r = buf["specularPower"]; r.Exists()) { // Assuming SkinnedShader_PS uses specularPower
                float gloss = 32.0f; material.Get(AI_MATKEY_SHININESS, gloss);
                r = gloss;
            }
        }
        else {
            // Original filling logic for dynamic Phong PS CBuf
            aiString texFileName_phong_fill; // Temp for filling
            bool hasGlossAlpha_phong_fill = false;
            if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName_phong_fill) == aiReturn_SUCCESS) {
                auto specTex = Texture::Resolve(gfx, rootPath + texFileName_phong_fill.C_Str(), 1);
                hasGlossAlpha_phong_fill = specTex->HasAlpha();
            }

            if (auto r = buf["materialColor"]; r.Exists()) { aiColor3D color = { 0.45f,0.45f,0.85f }; material.Get(AI_MATKEY_COLOR_DIFFUSE, color); r = reinterpret_cast<DirectX::XMFLOAT3&>(color); }
            buf["useGlossAlpha"].SetIfExists(hasGlossAlpha_phong_fill);
            buf["useSpecularMap"].SetIfExists(material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName_phong_fill) == aiReturn_SUCCESS);
            if (auto r = buf["specularColor"]; r.Exists()) { aiColor3D color = { 0.18f,0.18f,0.18f }; material.Get(AI_MATKEY_COLOR_SPECULAR, color); r = reinterpret_cast<DirectX::XMFLOAT3&>(color); }
            buf["specularWeight"].SetIfExists(1.0f);
            if (auto r = buf["specularGloss"]; r.Exists()) { float gloss = 8.0f; material.Get(AI_MATKEY_SHININESS, gloss); r = gloss; }
            buf["useNormalMap"].SetIfExists(material.GetTexture(aiTextureType_NORMALS, 0, &texFileName_phong_fill) == aiReturn_SUCCESS);
            buf["normalMapWeight"].SetIfExists(1.0f);
        }
        step.AddBindable(std::make_unique<Bind::CachingPixelConstantBufferEx>(gfx, std::move(buf), 1u)); // Slot b1

        phongOrSkinnedTechnique.AddStep(std::move(step));
        techniques.push_back(std::move(phongOrSkinnedTechnique));
    } // End of "Phong" / "Skinned" hybrid technique

    // --- Outline Technique --- (remains unchanged, uses this->vtxLayout)
    {
        Technique outline("Outline", false);
        // ... (your existing outline technique setup, ensuring its InputLayout::Resolve uses this->vtxLayout) ...
        {
            Step mask(1);
            auto pvs = VertexShader::Resolve(gfx, "Solid_VS.cso");
            auto pvsbc = pvs->GetBytecode();
            mask.AddBindable(std::move(pvs));
            mask.AddBindable(InputLayout::Resolve(gfx, this->vtxLayout, pvsbc));
            mask.AddBindable(std::make_shared<TransformCbuf>(gfx));
            outline.AddStep(std::move(mask));
        }
        {
            Step draw(2);
            auto pvs = VertexShader::Resolve(gfx, "Solid_VS.cso");
            auto pvsbc = pvs->GetBytecode();
            draw.AddBindable(std::move(pvs));
            draw.AddBindable(PixelShader::Resolve(gfx, "Solid_PS.cso"));
            {
                Dcb::RawLayout lay;
                lay.Add<Dcb::Float3>("materialColor");
                auto buf = Dcb::Buffer(std::move(lay));
                buf["materialColor"] = DirectX::XMFLOAT3{ 1.0f,0.4f,0.4f };
                draw.AddBindable(std::make_shared<Bind::CachingPixelConstantBufferEx>(gfx, buf, 1u));
            }
            draw.AddBindable(InputLayout::Resolve(gfx, this->vtxLayout, pvsbc));
            draw.AddBindable(std::make_shared<TransformCbuf>(gfx));
            outline.AddStep(std::move(draw));
        }
        techniques.push_back(std::move(outline));
    }
} // End of Material Constructor

// GetRequiredVertexLayout method also needs to be aware of this "char" model logic
Dvtx::VertexLayout Material::GetRequiredVertexLayout(const aiMesh& mesh) const
{
    // Start with a copy of the material's base layout (Pos, Norm, maybe UVs/Tangents based on general textures)
    Dvtx::VertexLayout layout = this->vtxLayout;

    // Check if this material is for a "char" model (using the same logic as in constructor)
    bool isCharModel = (this->modelPath.find("char") != std::string::npos);
    // Or more robustly: bool isCharModel = (path_parameter_passed_to_constructor.filename().string().find("char") != std::string::npos);
    // However, 'this->modelPath' is what the constructor used, so be consistent.

    if (isCharModel && mesh.HasBones()) {
        // If it's a char model AND the specific mesh has bones, ensure bone attributes are in the layout.
        if (!layout.Has(Dvtx::VertexLayout::BoneIDs)) {
            layout.Append(Dvtx::VertexLayout::BoneIDs);
        }
        if (!layout.Has(Dvtx::VertexLayout::BoneWeights)) {
            layout.Append(Dvtx::VertexLayout::BoneWeights);
        }
        // Ensure Texture2D is present for char models if SkinnedShader_VS/PS expect it
        if (!layout.Has(Dvtx::VertexLayout::Texture2D)) {
            layout.Append(Dvtx::VertexLayout::Texture2D);
        }
    }
    else if (mesh.HasBones()) {
        // It's a non-"char" model but it has bones.
        // Does our generic "Phong" shader path have a skinning variant we could switch to?
        // For now, this implementation doesn't have a generic _SKINNED suffix for Phong dynamically.
        // So, if it's not a "char" model, it won't get bone attributes added here by default.
        // This means non-"char" skinned models won't skin with this specific material setup.
        // This is a limitation of the simplified "char" check.
    }
    // If not char model and no bones, 'layout' remains 'this->vtxLayout' (correct for static Phong).

    return layout;
}

// ... (Rest of Material.cpp: ExtractVertices, ExtractIndices, MakeVertexBindable, MakeIndexBindable, GetTechniques, MakeMeshTag)
// MakeVertexBindable should use the dvtxBuffer passed from ModelComponent, which was created using GetRequiredVertexLayout.
// GetTechniques will now return a vector that might contain a "Phong" technique which is actually configured for skinning
// if it was a "char" model.


Dvtx::VertexBuffer Material::ExtractVertices( const aiMesh& mesh ) const noexcept
{
	return { vtxLayout,mesh };
}
std::vector<unsigned short> Material::ExtractIndices( const aiMesh& mesh ) const noexcept
{
	std::vector<unsigned short> indices;
	indices.reserve( mesh.mNumFaces * 3 );
	for( unsigned int i = 0; i < mesh.mNumFaces; i++ )
	{
		const auto& face = mesh.mFaces[i];
		assert( face.mNumIndices == 3 );
		indices.push_back( face.mIndices[0] );
		indices.push_back( face.mIndices[1] );
		indices.push_back( face.mIndices[2] );
	}
	return indices;
}
// In Material.cpp
std::shared_ptr<Bind::VertexBuffer> Material::MakeVertexBindable(
	Graphics& gfx,
	const aiMesh& mesh, 
	const Dvtx::VertexBuffer& dvtxBuffer
) const
{
	std::string uid = "$vb." + std::string(mesh.mName.C_Str()) + "." + dvtxBuffer.GetLayout().GetCode();
	return Bind::VertexBuffer::Resolve(gfx, uid, dvtxBuffer); // This now matches your VertexBuffer.h
}
std::shared_ptr<Bind::IndexBuffer> Material::MakeIndexBindable( Graphics& gfx,const aiMesh& mesh ) const noxnd
{
	return Bind::IndexBuffer::Resolve( gfx,MakeMeshTag( mesh ),ExtractIndices( mesh ) );
}
std::string Material::MakeMeshTag( const aiMesh& mesh ) const noexcept
{
	return modelPath + "%" + mesh.mName.C_Str();
}
std::vector<Technique> Material::GetTechniques() const noexcept
{
	return techniques;
}

 