#include "Mesh.h"
#include "Graphics.h" 
#include "Material.h" 
#include <assimp/mesh.h> 
#include <sstream>
#include "BindableCommon.h" // For Bind::...
#include "Vertex.h" // For Dvtx::...
#include "BindableCodex.h" // For Codex::Resolve
#include "GraphicsThrowMacros.h" // For ModelException

namespace dx = DirectX;

// ModelException should be defined elsewhere, but for completeness, I'll put it here.
// In a real project, this would go in its own header/cpp file.
//class ModelException : public CException
//{
//public:
//    ModelException(int line, const char* file, std::string note) noexcept
//        : CException(line, file), note(std::move(note))
//    {
//    }
//    const char* what() const noexcept override
//    {
//        std::ostringstream oss;
//        oss << CException::what() << std::endl << "[Note] " << GetNote();
//        whatBuffer = oss.str();
//        return whatBuffer.c_str();
//    }
//    const char* GetType() const noexcept override { return "Model Exception"; }
//    const std::string& GetNote() const noexcept { return note; }
//private:
//    std::string note;
//};

//================================================================================
// MESH CONSTRUCTOR (Completely Rewritten)
//================================================================================
Mesh::Mesh(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale)
{
    // 1. DYNAMICALLY BUILD THE VERTEX LAYOUT
    Dvtx::VertexLayout layout;
    if (mesh.HasPositions())
    {
        layout.Append(Dvtx::VertexLayout::Position3D);
    }
    if (mesh.HasNormals())
    {
        layout.Append(Dvtx::VertexLayout::Normal);
    }
    if (mesh.HasTextureCoords(0))
    {
        layout.Append(Dvtx::VertexLayout::Texture2D);
    }
    if (mesh.HasTangentsAndBitangents())
    {
        layout.Append(Dvtx::VertexLayout::Tangent);
        layout.Append(Dvtx::VertexLayout::Bitangent);
    }
    // NEW: Check for bone data
    if (mesh.HasBones())
    {
        layout.Append(Dvtx::VertexLayout::BoneIDs);
        layout.Append(Dvtx::VertexLayout::BoneWeights);
    }

    // 2. CREATE THE CPU-SIDE VERTEX BUFFER
    // The Dvtx::VertexBuffer constructor now handles all data extraction,
    // including the complex bone weights.
    Dvtx::VertexBuffer vbuf(std::move(layout), mesh);

    // 3. CREATE THE GPU-SIDE VERTEX BUFFER (Bindable)
    // We use the mesh name as part of the tag to uniquely identify it.
    std::string meshTag = std::string(mesh.mName.C_Str()) + std::to_string(mesh.mNumVertices);
    pVertices = Bind::VertexBuffer::Resolve(gfx, meshTag, vbuf);

    // 4. CREATE THE INDEX BUFFER
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
    pIndices = Bind::IndexBuffer::Resolve(gfx, meshTag, indices);

    // 5. CREATE THE TOPOLOGY
    pTopology = Bind::Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 6. INITIALIZE TECHNIQUES FROM THE MATERIAL
    for (auto& t : mat.GetTechniques())
    {
        AddTechnique(std::move(t));
    }

    // 7. INITIALIZE THE TRANSFORM
    dx::XMStoreFloat4x4(&transform, dx::XMMatrixScaling(scale, scale, scale));
}


void Mesh::Submit(dx::FXMMATRIX accumulatedTransform) const noxnd
{
    Submit(accumulatedTransform, nullptr);
}

void Mesh::Submit(dx::FXMMATRIX accumulatedTransform, const std::vector<dx::XMMATRIX>* pBoneTransforms) const noxnd
{
    dx::XMStoreFloat4x4(&transform, accumulatedTransform);
    if (pBoneTransforms)
    {
        boneTransforms = *pBoneTransforms;
    }
    else
    {
        boneTransforms.clear();
    }
    Drawable::Submit();
}

const std::vector<DirectX::XMMATRIX>* Mesh::GetBoneTransformsPtr() const noexcept
{
    // Only return a valid pointer if there are transforms,
    // signalling to the cbuf that it should bind.
    return boneTransforms.empty() ? nullptr : &boneTransforms;
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
    return DirectX::XMLoadFloat4x4(&transform);
}