#include "ModelComponent.h"
#include "Mesh.h" // Include the actual Mesh definition
#include "Node.h" // Include the Scene Node definition
#include "Graphics.h"
#include "Surface.h"
#include "BindableCommon.h" // For specific bindables like Texture, Sampler etc.
#include "Vertex.h"         // For Vertex layout

// Assimp includes (only needed in cpp)
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// ImGui include (only needed in cpp because of pImpl)
#include "imgui/imgui.h"
#include <unordered_map>
#include <sstream>
#include <filesystem> // For path manipulation

namespace dx = DirectX;

// --- Define TransformParameters OUTSIDE and BEFORE ModelControlWindow ---
// Helper struct for transform params within the map
struct TransformParameters
{
    float roll = 0.0f;
    float pitch = 0.0f;
    float yaw = 0.0f;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};


// --- PImpl Class for ImGui Window ---
// Put this definition inside the .cpp file
class ModelControlWindow
{
public:
    // --- Correct the signature for the 'transforms' map parameter ---
    void Show(const char* windowName, const ModelInternalNode& root, ModelInternalNode*& pSelectedNode,
        std::unordered_map<int, TransformParameters>& transforms) noexcept // Corrected type
    {
        windowName = windowName ? windowName : "Model Controls";
        if (ImGui::Begin(windowName))
        {
            ImGui::Columns(2, nullptr, true);

            int nodeIndexTracker = 0;
            root.ShowTree(nodeIndexTracker, pSelectedNode);

            ImGui::NextColumn();
            if (pSelectedNode != nullptr)
            {
                const int selectedId = pSelectedNode->GetId();
                // --- This line should now work correctly ---
                auto& transform = transforms[selectedId];

                // --- Use GetName() method (see fix #3) ---
                ImGui::Text("Node: %s", pSelectedNode->GetName().c_str());
                ImGui::Separator();
                ImGui::Text("Orientation (Local Offset)");
                ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
                ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
                ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);
                ImGui::Text("Position (Local Offset)");
                ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f);
                ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f);
                ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f);

                // Apply the transform to the node
                // --- Fix Rotation Order (Pitch, Yaw, Roll for XMMatrixRotationRollPitchYaw) ---
                pSelectedNode->SetAppliedTransform(
                    dx::XMMatrixRotationRollPitchYaw(transform.pitch, transform.yaw, transform.roll) * // Correct order
                    dx::XMMatrixTranslation(transform.x, transform.y, transform.z)
                );
            }
            else
            {
                ImGui::Text("Select a node in the tree.");
            }
        }
        ImGui::End();
    }

    // Store selected node and transforms here, managed by ModelComponent instance
    ModelInternalNode* pSelectedNode = nullptr;
    // --- Ensure map type uses the now globally defined TransformParameters ---
    std::unordered_map<int, TransformParameters> transforms;
};

// ... (rest of ModelComponent.cpp)


// --- ModelInternalNode Implementation --- (Mostly from original Node inside Model)

ModelInternalNode::ModelInternalNode(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const dx::XMMATRIX& transform_in) noxnd
    : id(id), name(name), meshPtrs(std::move(meshPtrs))
{
    dx::XMStoreFloat4x4(&transform, transform_in); // Store the transform from the file
    dx::XMStoreFloat4x4(&appliedTransform, dx::XMMatrixIdentity()); // Init applied transform
}

void ModelInternalNode::AddChild(std::unique_ptr<ModelInternalNode> pChild) noxnd
{
    assert(pChild);
    childPtrs.push_back(std::move(pChild));
}

// Draw method for the *internal* node structure
void ModelInternalNode::Draw(Graphics& gfx, dx::FXMMATRIX accumulatedTransform) const noxnd
{
    // Combine parent's accumulated transform, this node's file transform, and the ImGui-applied transform
    const auto modelNodeTransform =
        dx::XMLoadFloat4x4(&appliedTransform) * // Apply ImGui offset first
        dx::XMLoadFloat4x4(&transform) *        // Then node's own transform from file
        accumulatedTransform;                   // Then parent's accumulated transform

    // Draw meshes associated with this node using the final calculated transform
    for (const auto pm : meshPtrs)
    {
        pm->Draw(gfx, modelNodeTransform);
    }

    // Recursively draw children, passing the combined transform
    for (const auto& pc : childPtrs)
    {
        pc->Draw(gfx, modelNodeTransform);
    }
}

int ModelInternalNode::GetId() const noexcept
{
    return id;
}

void ModelInternalNode::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept
{
    dx::XMStoreFloat4x4(&appliedTransform, transform);
}

// ShowTree function for ImGui - adapted to use the pSelectedNode reference
void ModelInternalNode::ShowTree(int& nodeIndexTracker, ModelInternalNode*& pSelectedNode) const noexcept
{
    nodeIndexTracker++; // Increment tracker for unique ID

    // Check if this node is the selected one
    const int currentId = GetId();
    const bool isSelected = (pSelectedNode != nullptr) && (pSelectedNode->GetId() == currentId);

    // Node flags for ImGui
    const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow |
        (isSelected ? ImGuiTreeNodeFlags_Selected : 0) |
        (childPtrs.empty() ? ImGuiTreeNodeFlags_Leaf : 0);

    // Use node ID and tracker for a unique ImGui ID
    const bool expanded = ImGui::TreeNodeEx(
        (void*)(intptr_t)(currentId + nodeIndexTracker * 10000), // Combine ID with tracker for uniqueness
        node_flags, name.c_str()
    );

    // Handle selection click
    if (ImGui::IsItemClicked())
    {
        pSelectedNode = const_cast<ModelInternalNode*>(this); // Update the selected node reference
    }

    // Recurse for children if expanded
    if (expanded)
    {
        for (const auto& pChild : childPtrs)
        {
            pChild->ShowTree(nodeIndexTracker, pSelectedNode); // Pass reference down
        }
        ImGui::TreePop();
    }
}

// --- ModelComponent Implementation ---

ModelComponent::ModelComponent(Node* owner, Graphics& gfx, const std::string& modelFile)
    : Component(owner), pControlWindow(std::make_unique<ModelControlWindow>())
{
    Assimp::Importer importer;
    const auto pScene = importer.ReadFile(modelFile.c_str(),
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_ConvertToLeftHanded | // Ensure left-handed coordinates
        aiProcess_GenNormals |          // Generate normals if missing
        aiProcess_CalcTangentSpace     // Optional: if using normal maps
    );

    if (!pScene || !pScene->mRootNode || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
    {
        throw ModelException(__LINE__, __FILE__, "Failed to load model file: " + modelFile + " | Assimp error: " + importer.GetErrorString());
    }

    // Extract path from model file for loading textures relative to it
    std::filesystem::path filePath(modelFile);
    std::string modelPath = filePath.parent_path().string() + "\\"; // Needs trailing slash


    // Parse materials and meshes first
    meshPtrs.reserve(pScene->mNumMeshes);
    for (unsigned int i = 0; i < pScene->mNumMeshes; ++i)
    {
        meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials, modelPath));
    }

    // Parse node hierarchy recursively
    int nextId = 0; // Start ID counter for internal nodes
    pRootInternal = ParseNodeRecursive(nextId, *pScene->mRootNode);
}

ModelComponent::~ModelComponent() noexcept = default; // Needed because of unique_ptr<ModelControlWindow> pImpl

void ModelComponent::Draw(Graphics& gfx, dx::FXMMATRIX worldTransform) const noxnd
{
    // Start drawing the internal hierarchy, passing the owning Scene Node's world transform
    // as the initial accumulated transform.
    if (pRootInternal)
    {
        pRootInternal->Draw(gfx, worldTransform);
    }
}


std::unique_ptr<ModelInternalNode> ModelComponent::ParseNodeRecursive(int& nextId, const aiNode& node) noexcept
{
    namespace dx = DirectX;
    // Get the node's transform relative to its parent (and transpose for DirectX)
    const auto nodeTransform = dx::XMMatrixTranspose(dx::XMLoadFloat4x4(
        reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)
    ));

    // Gather pointers to the meshes this node uses
    std::vector<Mesh*> currentNodeMeshPtrs;
    currentNodeMeshPtrs.reserve(node.mNumMeshes);
    for (unsigned int i = 0; i < node.mNumMeshes; ++i)
    {
        const auto meshIndex = node.mMeshes[i];
        // Ensure meshIndex is valid before accessing meshPtrs
        if (meshIndex < meshPtrs.size())
        {
             currentNodeMeshPtrs.push_back(meshPtrs[meshIndex].get());
        } else {
            // Handle error or warning: mesh index out of bounds
            OutputDebugStringA(("Warning: Mesh index out of bounds in node " + std::string(node.mName.C_Str()) + "\n").c_str());
        }

    }

    // Create the internal node object
    auto pNode = std::make_unique<ModelInternalNode>(nextId++, node.mName.C_Str(), std::move(currentNodeMeshPtrs), nodeTransform);

    // Recursively parse children
    for (unsigned int i = 0; i < node.mNumChildren; ++i)
    {
        pNode->AddChild(ParseNodeRecursive(nextId, *node.mChildren[i]));
    }

    return pNode;
}

std::unique_ptr<Mesh> ModelComponent::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::string& modelPath)
{
    using Dvtx::VertexLayout;
    namespace dx = DirectX;

    // Create Vertex Buffer Layout
    VertexLayout layout = VertexLayout{}
        .Append(VertexLayout::Position3D)
        .Append(VertexLayout::Normal);

    bool hasTexCoords = mesh.HasTextureCoords(0);
    if (hasTexCoords)
    {
        layout.Append(VertexLayout::Texture2D);
    }
    // ... Add tangent etc. if needed

    Dvtx::VertexBuffer vbuf(std::move(layout)); // Construct with layout

    // --- Remove vbuf.Reserve call ---
    // vbuf.Reserve(mesh.mNumVertices); // REMOVE THIS LINE

    for (unsigned int i = 0; i < mesh.mNumVertices; ++i)
    {
        // --- Construct Vertex directly in EmplaceBack ---
        // Remove: Dvtx::Vertex vertex;
        // Remove: vertex.Attr<...>() = ...;

        // Assuming EmplaceBack can take the necessary arguments based on the layout.
        // You might need to adjust this based on your actual Vertex/VertexBuffer implementation.
        // This example passes pointers to the data. Your EmplaceBack might need copies.
        if (hasTexCoords) {
            vbuf.EmplaceBack(
                *reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i]),
                *reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
                *reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
            );
        }
        else {
            vbuf.EmplaceBack(
                *reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i]),
                *reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i])
                // No texture coordinates passed
            );
        }
        // If tangents/bitangents are added, include them here too.
    }

    // ... (rest of ParseMesh) ...

    std::vector<std::unique_ptr<Bind::Bindable>> bindablePtrs;

    // --- Vertex Buffer: Pass the already filled vbuf ---
    bindablePtrs.push_back(std::make_unique<Bind::VertexBuffer>(gfx, vbuf));

    // ... Index Buffer, Shaders, Textures etc.
     // Create Index Buffer Data
    std::vector<unsigned short> indices;
    indices.reserve((size_t)mesh.mNumFaces * 3);
    for (unsigned int i = 0; i < mesh.mNumFaces; ++i)
    {
        const auto& face = mesh.mFaces[i];
        assert(face.mNumIndices == 3); // Ensure triangles
        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);
    }
    bindablePtrs.push_back(std::make_unique<Bind::IndexBuffer>(gfx, indices));

    // Shaders (Assuming Phong Shaders exist)
    auto pvs = std::make_unique<Bind::VertexShader>(gfx, L"PhongVS.cso");
    auto pvsbc = pvs->GetBytecode(); // Needed for Input Layout
    bindablePtrs.push_back(std::move(pvs));

    // Input Layout (Uses the layout from vbuf)
    bindablePtrs.push_back(std::make_unique<Bind::InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), pvsbc));

    // ... (rest of material/texture/shader setup remains the same)
      // --- Create Bindables ---
    //std::vector<std::unique_ptr<Bind::Bindable>> bindablePtrs; // Already declared above

    bool hasSpecularMap = false;
    float shininess = 40.0f;
    aiColor3D diffuseColor(1.0f, 1.0f, 1.0f);
    aiColor3D specularColor(1.0f, 1.0f, 1.0f);


    if (mesh.mMaterialIndex >= 0)
    {
        const auto& material = *pMaterials[mesh.mMaterialIndex];

        aiString texPath;
        // Diffuse Texture
        if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == aiReturn_SUCCESS)
        {
            bindablePtrs.push_back(std::make_unique<Bind::Texture>(gfx, Surface::FromFile(modelPath + texPath.C_Str()), 0u)); // Slot 0
        }
        else
        {
            material.Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
            // Potentially load a default white texture or handle via shader constants
        }


        // Specular Texture (Check if exists)
        if (material.GetTexture(aiTextureType_SPECULAR, 0, &texPath) == aiReturn_SUCCESS)
        {
            bindablePtrs.push_back(std::make_unique<Bind::Texture>(gfx, Surface::FromFile(modelPath + texPath.C_Str()), 1u)); // Slot 1
            hasSpecularMap = true;
        }
        else
        {
            material.Get(AI_MATKEY_COLOR_SPECULAR, specularColor); // Get specular color if no map
            material.Get(AI_MATKEY_SHININESS, shininess); // Get shininess if no map
        }

        // Add Sampler (common for textures)
        bindablePtrs.push_back(std::make_unique<Bind::Sampler>(gfx));

        // Choose Pixel Shader based on material properties
        if (hasSpecularMap) {
            bindablePtrs.push_back(std::make_unique<Bind::PixelShader>(gfx, L"PhongPSSpecMap.cso")); // Needs a shader that samples specular map
        }
        else {
            bindablePtrs.push_back(std::make_unique<Bind::PixelShader>(gfx, L"PhongPS.cso")); // Basic Phong PS

            // If no specular map, set up material constants
            struct PSMaterialConstant
            {
                dx::XMFLOAT3 specularColor = { 1.0f, 1.0f, 1.0f }; // 12 bytes
                float specularPower;                               //  4 bytes
                // Total size = 16 bytes (Multiple of 16)
            } pmc;
            pmc.specularColor = { specularColor.r, specularColor.g, specularColor.b };
            pmc.specularPower = shininess > 0.0f ? shininess : 1.0f;

             // Bind constant buffer to appropriate slot (e.g., slot 1)
            bindablePtrs.push_back(std::make_unique<Bind::PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));
        }

        // Add Normal Map Texture if present
        // if (material.GetTexture(aiTextureType_NORMALS, 0, &texPath) == aiReturn_SUCCESS) { ... }

    }
    else {
        // Default material if none specified
        bindablePtrs.push_back(std::make_unique<Bind::PixelShader>(gfx, L"PhongPS.cso"));
        // Bind default material constants... (Similar to above)
        struct PSMaterialConstant
        {
            dx::XMFLOAT3 specularColor = { 1.0f, 1.0f, 1.0f }; // 12 bytes
            float specularPower = 40.0f;                      //  4 bytes
            // Total size = 16 bytes (Multiple of 16)
        } pmc;
        bindablePtrs.push_back(std::make_unique<Bind::PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));
        bindablePtrs.push_back(std::make_unique<Bind::Sampler>(gfx));
    }


    return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
}

void ModelComponent::ShowWindow(const char* windowName) noexcept
{
    if (pRootInternal && pControlWindow)
    {
        // The types passed here should now match the corrected Show signature
        pControlWindow->Show(windowName, *pRootInternal, pControlWindow->pSelectedNode, pControlWindow->transforms);
    }
}