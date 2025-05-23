#include "Material.h"
#include "DynamicConstant.h"
#include "ConstantBuffersEx.h"
#include <scene.h>
#include "Vertex.h"

Material::Material( Graphics& gfx,const aiMaterial& material,const std::filesystem::path& path ) noxnd
	:
	modelPath( path.string() )
{
	using namespace Bind;
	const auto rootPath = path.parent_path().string() + "\\";
	{
		aiString tempName;
		material.Get( AI_MATKEY_NAME,tempName );
		name = tempName.C_Str();
	}
	// phong technique
	{
		Technique phong{ "Phong" };
		Step step( 0 );
		std::string shaderCode = "Phong";
		aiString texFileName;

		// common (pre)
		vtxLayout.Append( Dvtx::VertexLayout::Position3D );
		vtxLayout.Append( Dvtx::VertexLayout::Normal );
		Dcb::RawLayout pscLayout;
		bool hasTexture = false;
		bool hasGlossAlpha = false;

		// diffuse
		{
			bool hasAlpha = false;
			if( material.GetTexture( aiTextureType_DIFFUSE,0,&texFileName ) == aiReturn_SUCCESS )
			{
				hasTexture = true;
				shaderCode += "Dif";
				vtxLayout.Append( Dvtx::VertexLayout::Texture2D );
				auto tex = Texture::Resolve( gfx,rootPath + texFileName.C_Str() );
				if( tex->HasAlpha() )
				{
					hasAlpha = true;
					shaderCode += "Msk";
				}
				step.AddBindable( std::move( tex ) );
			}
			else
			{
				pscLayout.Add<Dcb::Float3>( "materialColor" );
			}
			step.AddBindable( Rasterizer::Resolve( gfx,hasAlpha ) );
		}
		// specular
		{
			if( material.GetTexture( aiTextureType_SPECULAR,0,&texFileName ) == aiReturn_SUCCESS )
			{
				hasTexture = true;
				shaderCode += "Spc";
				vtxLayout.Append( Dvtx::VertexLayout::Texture2D );
				auto tex = Texture::Resolve( gfx,rootPath + texFileName.C_Str(),1 );
				hasGlossAlpha = tex->HasAlpha();
				step.AddBindable( std::move( tex ) );
				pscLayout.Add<Dcb::Bool>( "useGlossAlpha" );
				pscLayout.Add<Dcb::Bool>( "useSpecularMap" );
			}
			pscLayout.Add<Dcb::Float3>( "specularColor" );
			pscLayout.Add<Dcb::Float>( "specularWeight" );
			pscLayout.Add<Dcb::Float>( "specularGloss" );
		}
		// normal
		{
			if( material.GetTexture( aiTextureType_NORMALS,0,&texFileName ) == aiReturn_SUCCESS )
			{
				hasTexture = true;
				shaderCode += "Nrm";
				vtxLayout.Append( Dvtx::VertexLayout::Texture2D );
				vtxLayout.Append( Dvtx::VertexLayout::Tangent );
				vtxLayout.Append( Dvtx::VertexLayout::Bitangent );
				step.AddBindable( Texture::Resolve( gfx,rootPath + texFileName.C_Str(),2 ) );
				pscLayout.Add<Dcb::Bool>( "useNormalMap" );
				pscLayout.Add<Dcb::Float>( "normalMapWeight" );
			}
		}
		// common (post)
		{
			step.AddBindable( std::make_shared<TransformCbuf>( gfx,0u ) );
			step.AddBindable( Blender::Resolve( gfx,false ) );
			auto pvs = VertexShader::Resolve( gfx,shaderCode + "_VS.cso" );
			auto pvsbc = pvs->GetBytecode();
			step.AddBindable( std::move( pvs ) );
			step.AddBindable( PixelShader::Resolve( gfx,shaderCode + "_PS.cso" ) );
			step.AddBindable( InputLayout::Resolve( gfx,vtxLayout,pvsbc ) );
			if( hasTexture )
			{
				step.AddBindable( Bind::Sampler::Resolve( gfx ) );
			}
			// PS material params (cbuf)
			Dcb::Buffer buf{ std::move( pscLayout ) };
			if( auto r = buf["materialColor"]; r.Exists() )
			{
				aiColor3D color = { 0.45f,0.45f,0.85f };
				material.Get( AI_MATKEY_COLOR_DIFFUSE,color );
				r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
			}
			buf["useGlossAlpha"].SetIfExists( hasGlossAlpha );
			buf["useSpecularMap"].SetIfExists( true );
			if( auto r = buf["specularColor"]; r.Exists() )
			{
				aiColor3D color = { 0.18f,0.18f,0.18f };
				material.Get( AI_MATKEY_COLOR_SPECULAR,color );
				r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
			}
			buf["specularWeight"].SetIfExists( 1.0f );
			if( auto r = buf["specularGloss"]; r.Exists() )
			{
				float gloss = 8.0f;
				material.Get( AI_MATKEY_SHININESS,gloss );
				r = gloss;
			}
			buf["useNormalMap"].SetIfExists( true );
			buf["normalMapWeight"].SetIfExists( 1.0f );
			step.AddBindable( std::make_unique<Bind::CachingPixelConstantBufferEx>( gfx,std::move( buf ),1u ) );
		}
		phong.AddStep( std::move( step ) );
		techniques.push_back( std::move( phong ) );
	}
	// outline technique
	{
		Technique outline( "Outline",false );
		{
			Step mask( 1 );

			auto pvs = VertexShader::Resolve( gfx,"Solid_VS.cso" );
			auto pvsbc = pvs->GetBytecode();
			mask.AddBindable( std::move( pvs ) );

			// TODO: better sub-layout generation tech for future consideration maybe
			mask.AddBindable( InputLayout::Resolve( gfx,vtxLayout,pvsbc ) );

			mask.AddBindable( std::make_shared<TransformCbuf>( gfx ) );

			// TODO: might need to specify rasterizer when doubled-sided models start being used

			outline.AddStep( std::move( mask ) );
		}
		{
			Step draw( 2 );

			// these can be pass-constant (tricky due to layout issues)
			auto pvs = VertexShader::Resolve( gfx,"Solid_VS.cso" );
			auto pvsbc = pvs->GetBytecode();
			draw.AddBindable( std::move( pvs ) );

			// this can be pass-constant
			draw.AddBindable( PixelShader::Resolve( gfx,"Solid_PS.cso" ) );

			{
				Dcb::RawLayout lay;
				lay.Add<Dcb::Float3>( "materialColor" );
				auto buf = Dcb::Buffer( std::move( lay ) );
				buf["materialColor"] = DirectX::XMFLOAT3{ 1.0f,0.4f,0.4f };
				draw.AddBindable( std::make_shared<Bind::CachingPixelConstantBufferEx>( gfx,buf,1u ) );
			}

			// TODO: better sub-layout generation tech for future consideration maybe
			draw.AddBindable( InputLayout::Resolve( gfx,vtxLayout,pvsbc ) );

			draw.AddBindable( std::make_shared<TransformCbuf>( gfx ) );

			// TODO: might need to specify rasterizer when doubled-sided models start being used

			outline.AddStep( std::move( draw ) );
		}
		techniques.push_back( std::move( outline ) );
	}
}
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

Dvtx::VertexLayout Material::GetRequiredVertexLayout(const aiMesh& mesh) const
{
	Dvtx::VertexLayout layout;
	layout.Append(Dvtx::VertexLayout::Position3D); // Assuming all your renderable meshes use 3D positions

	if (mesh.HasNormals()) {
		layout.Append(Dvtx::VertexLayout::Normal);
	}
	if (mesh.HasTextureCoords(0)) { // Check for the first UV channel
		layout.Append(Dvtx::VertexLayout::Texture2D);
	}
	// Add other standard attributes based on aiMesh properties and material needs
	if (mesh.HasTangentsAndBitangents()) {
		// Check if your material's techniques actually USE tangents/bitangents
		// For now, let's assume if they exist, we add them.
		layout.Append(Dvtx::VertexLayout::Tangent);
		layout.Append(Dvtx::VertexLayout::Bitangent);
	}
	// Example for vertex colors:
	// if (mesh.HasVertexColors(0)) { // Check for first vertex color channel
	//     layout.Append(Dvtx::VertexLayout::BGRAColor); // Or Float3Color/Float4Color
	// }


	// --- Logic for Bone Data ---
	bool materialActuallyUsesSkinning = false;

	// TODO: Implement a robust way to check if THIS material instance's techniques
	//       are designed for skinned animation.
	// This could involve:
	//  - A boolean flag in the Material class itself (e.g., `bool m_isSkinnedMaterial;`)
	//  - Checking the shaders used by its Techniques (e.g., do they have a "SKINNED" permutation,
	//    or do they bind a bone matrix constant buffer?)
	//  - Checking if any Technique has a Step that requires BLENDINDICES/BLENDWEIGHT semantics.

	// For now, we'll use a simplified approach: if the mesh has bones,
	// we'll assume this material *might* want to use them.
	// This is not ideal long-term, as a static material might be applied to a skinned mesh
	// without intending to skin it.
	if (mesh.HasBones()) {
		materialActuallyUsesSkinning = true; // Placeholder logic
	}

	if (materialActuallyUsesSkinning) {
		layout.Append(Dvtx::VertexLayout::BoneIDs);
		layout.Append(Dvtx::VertexLayout::BoneWeights);
	}

	return layout;
}