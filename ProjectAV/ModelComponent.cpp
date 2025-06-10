// ModelComponent.cpp
#include "ModelComponent.h"
#include "Mesh.h"
#include "Node.h"
#include "Graphics.h"
#include "Material.h"
#include "FrameCommander.h"
#include <iostream> // For std::hex if used in PrintMatrix2, otherwise not strictly needed

#include <assimp/DefaultLogger.hpp>
#include <assimp/Logger.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "imgui/imgui.h" // If ModelControlWindow is used
#include <unordered_map>
#include <sstream>
#include <filesystem>
#include <stdexcept> // For ModelException if it's defined elsewhere, or use std::runtime_error
// #include <scene.h> // This seems like an internal Assimp header, probably not needed directly by you
#include "Bone.h"
#include "Vertex.h"
#include <DirectXMath.h>
#include <algorithm>
#include <iomanip> // For std::fixed and std::setprecision in PrintMatrix2


namespace dx = DirectX;

 

// Forward declare or include ModelControlWindow definition if it's separate
struct TransformParameters
{
    float roll = 0.0f; float pitch = 0.0f; float yaw = 0.0f;
    float x = 0.0f;    float y = 0.0f;     float z = 0.0f;
};

class ModelControlWindow { /* Your ImGui window implementation */
public:
    void Show(const char* windowName, const ModelInternalNode& root, ModelInternalNode*& pSelectedNode,
        std::unordered_map<int, TransformParameters>& transforms) noexcept {
        // Minimal implementation for placeholder
        if (ImGui::Begin(windowName)) { ImGui::Text("Model Controls Placeholder"); } ImGui::End();
    }
    ModelInternalNode* pSelectedNode = nullptr;
    std::unordered_map<int, TransformParameters> transforms;
};


// PrintMatrix2 (ensure it's available or use your version)
void PrintMatrix2(const std::string& matrixName, const DirectX::XMMATRIX& matrix) {
    std::ostringstream oss;
    oss << "Matrix: " << matrixName << std::fixed << std::setprecision(3) << "\n";
    DirectX::XMFLOAT4X4 matData;
    DirectX::XMStoreFloat4x4(&matData, matrix);
    oss << "[" << matData._11 << ", " << matData._12 << ", " << matData._13 << ", " << matData._14 << "]\n";
    oss << "[" << matData._21 << ", " << matData._22 << ", " << matData._23 << ", " << matData._24 << "]\n";
    oss << "[" << matData._31 << ", " << matData._32 << ", " << matData._33 << ", " << matData._34 << "]\n";
    oss << "[" << matData._41 << ", " << matData._42 << ", " << matData._43 << ", " << matData._44 << "]\n\n";
    OutputDebugStringA(oss.str().c_str());
}

ModelInternalNode::ModelInternalNode(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const dx::XMMATRIX& transform_in) noxnd
    : id(id), name(name), meshPtrs(std::move(meshPtrs)) {
    dx::XMStoreFloat4x4(&transform, transform_in);
    dx::XMStoreFloat4x4(&appliedTransform, dx::XMMatrixIdentity());
}
void ModelInternalNode::AddChild(std::unique_ptr<ModelInternalNode> pChild) noxnd {
    assert(pChild); childPtrs.push_back(std::move(pChild));
}
void ModelInternalNode::Submit(FrameCommander& frame, Graphics& gfx, dx::FXMMATRIX accumulatedTransform) const noxnd {
    const auto modelNodeTransform = dx::XMLoadFloat4x4(&appliedTransform) * dx::XMLoadFloat4x4(&transform) * accumulatedTransform;
    for (const auto pm : meshPtrs) { pm->Submit(frame, modelNodeTransform); }
    for (const auto& pc : childPtrs) { pc->Submit(frame, gfx, modelNodeTransform); }
}
int ModelInternalNode::GetId() const noexcept { return id; }
void ModelInternalNode::SetAppliedTransform(DirectX::FXMMATRIX transform_in) noexcept { dx::XMStoreFloat4x4(&appliedTransform, transform_in); }
void ModelInternalNode::ShowTree(int& nodeIndexTracker, ModelInternalNode*& pSelectedNode_ref) const noexcept {
    nodeIndexTracker++;
    const bool isSelected = (pSelectedNode_ref != nullptr) && (pSelectedNode_ref->GetId() == id);
    const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow | (isSelected ? ImGuiTreeNodeFlags_Selected : 0) | (childPtrs.empty() ? ImGuiTreeNodeFlags_Leaf : 0);
    const bool expanded = ImGui::TreeNodeEx((void*)(intptr_t)(id + nodeIndexTracker * 10000), node_flags, name.c_str());
    if (ImGui::IsItemClicked()) { pSelectedNode_ref = const_cast<ModelInternalNode*>(this); }
    if (expanded) {
        for (const auto& pChild : childPtrs) { pChild->ShowTree(nodeIndexTracker, pSelectedNode_ref); }
        ImGui::TreePop();
    }
}

void LogAssimpNodeHierarchy(const aiNode* pNode, const std::unordered_map<std::string, BoneInfo>& boneInfoMap, int indentLevel = 0); // Forward declare if definition is later


ModelComponent::ModelComponent(Node* owner, Graphics& gfx, const std::string& modelFile, float scale)
    : Component(owner, "ModelComponent"),
    pControlWindow(std::make_unique<ModelControlWindow>()),
    m_BoneCounter(0) // Ensure m_BoneCounter is initialized
{
    std::ostringstream oss_constructor;
    oss_constructor << "ModelComponent: Loading model '" << modelFile << "' with scale " << scale << "\n";
    OutputDebugStringA(oss_constructor.str().c_str());

    Assimp::Importer importer;
    Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE, aiDefaultLogStream_DEBUGGER);

    unsigned int importFlags = aiProcess_ConvertToLeftHanded | aiProcess_Triangulate;
    // Add other flags back LATER, once skinning works with minimal flags
    // importFlags |= aiProcess_JoinIdenticalVertices;
    // importFlags |= aiProcess_GenNormals; // Usually not needed for GLB from Blender
    // importFlags |= aiProcess_CalcTangentSpace; // If you need tangents for normal mapping
    // importFlags |= aiProcess_LimitBoneWeights; // IMPORTANT for skinning - add this back once basic is working

    oss_constructor.str(""); oss_constructor.clear();
    oss_constructor << "ModelComponent: Attempting to load with flags: 0x" << std::hex << importFlags << std::dec << "\n";
    OutputDebugStringA(oss_constructor.str().c_str());

    const aiScene* pScene = importer.ReadFile(modelFile.c_str(), importFlags);

    if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode) {
        oss_constructor.str(""); oss_constructor.clear();
        oss_constructor << "Assimp error after ReadFile: " << importer.GetErrorString() << "\n";
        OutputDebugStringA(oss_constructor.str().c_str());
        Assimp::DefaultLogger::kill();
        throw ModelException(__LINE__, __FILE__, "Assimp error: " + std::string(importer.GetErrorString()));
    }
    oss_constructor.str(""); oss_constructor.clear();
    oss_constructor << "ModelComponent: Successfully called ReadFile.\n";
    OutputDebugStringA(oss_constructor.str().c_str());

    std::filesystem::path filePath(modelFile);
    std::vector<Material> materials;
    materials.reserve(pScene->mNumMaterials);
    for (size_t i = 0; i < pScene->mNumMaterials; ++i) {
        materials.emplace_back(gfx, *pScene->mMaterials[i], filePath);
    }

    oss_constructor.str(""); oss_constructor.clear();
    oss_constructor << "ModelComponent: Processing " << pScene->mNumMeshes << " meshes...\n";
    OutputDebugStringA(oss_constructor.str().c_str());

    meshPtrs.reserve(pScene->mNumMeshes);
    for (unsigned int i = 0; i < pScene->mNumMeshes; ++i) {
        const auto& assimpMesh = *pScene->mMeshes[i];
        const auto& material = materials[assimpMesh.mMaterialIndex];

        // ... (mesh logging) ...

        Dvtx::VertexLayout dvtxLayout = material.GetRequiredVertexLayout(assimpMesh);
        Dvtx::VertexBuffer dvtxBuffer(dvtxLayout, assimpMesh);

        if (dvtxLayout.Has(Dvtx::VertexLayout::Position3D)) {
            for (size_t vIdx = 0; vIdx < dvtxBuffer.Size(); ++vIdx) {
                auto& pos = dvtxBuffer[vIdx].Attr<Dvtx::VertexLayout::Position3D>();
                pos.x *= scale; pos.y *= scale; pos.z *= scale;
            }
        }

        if (dvtxLayout.Has(Dvtx::VertexLayout::BoneIDs) && dvtxLayout.Has(Dvtx::VertexLayout::BoneWeights)) {
            if (assimpMesh.HasBones()) {
                ExtractBoneWeightForVertices(dvtxBuffer, assimpMesh, DirectX::XMMatrixIdentity());
                // ... (vertex weight debug logging) ...
            }
        }

        auto pD3DVertexBuffer = material.MakeVertexBindable(gfx, assimpMesh, dvtxBuffer);
        auto pD3DIndexBuffer = material.MakeIndexBindable(gfx, assimpMesh);
        meshPtrs.push_back(std::make_unique<Mesh>(this->pOwner, gfx, pD3DVertexBuffer, pD3DIndexBuffer, material));
    }

    // --- Build ModelInternalNode hierarchy ---
    int nextId = 0;
    pRootInternal = ParseNodeRecursive(nextId, *pScene->mRootNode, scale);

    // --- Calculate and store bone offset matrices ---
    if (pRootInternal) {
        CalculateBoneOffsetsRecursive(pRootInternal.get(), DirectX::XMMatrixIdentity());
    }

    // --- DEBUG Logging (After all calculations) ---
#ifdef _DEBUG
    std::ostringstream oss_bones_final;
    oss_bones_final << "\nModelComponent: FINAL Bone Information (m_BoneInfoMap) AFTER ALL CALCULATIONS:\n";
    // Sort bone names for consistent logging order (optional, but nice for comparison)
    std::vector<std::string> sortedBoneNames;
    for (const auto& pair : m_BoneInfoMap) sortedBoneNames.push_back(pair.first);
    std::sort(sortedBoneNames.begin(), sortedBoneNames.end());

    for (const auto& boneName : sortedBoneNames) {
        const auto& boneInfo = m_BoneInfoMap.at(boneName);
        oss_bones_final << "  Bone Name: '" << boneName << "', Assigned ID: " << boneInfo.id << "\n";
        DirectX::XMMATRIX tempOffset = DirectX::XMLoadFloat4x4(&boneInfo.offsetMatrix);

        // Use your PrintMatrix2 or similar to format the matrix output nicely into oss_bones_final
        std::ostringstream matrix_ss_final; // Temp stream for one matrix
        matrix_ss_final << "    Offset for " << boneName << ":" << std::fixed << std::setprecision(3) << "\n";
        DirectX::XMFLOAT4X4 matData_final;
        DirectX::XMStoreFloat4x4(&matData_final, tempOffset);
        matrix_ss_final << "    [" << matData_final._11 << ", " << matData_final._12 << ", " << matData_final._13 << ", " << matData_final._14 << "]\n";
        matrix_ss_final << "    [" << matData_final._21 << ", " << matData_final._22 << ", " << matData_final._23 << ", " << matData_final._24 << "]\n";
        matrix_ss_final << "    [" << matData_final._31 << ", " << matData_final._32 << ", " << matData_final._33 << ", " << matData_final._34 << "]\n";
        matrix_ss_final << "    [" << matData_final._41 << ", " << matData_final._42 << ", " << matData_final._43 << ", " << matData_final._44 << "]\n\n";
        oss_bones_final << matrix_ss_final.str();
    }
    oss_bones_final << "Total unique bones (m_BoneCounter): " << m_BoneCounter << "\n";
    OutputDebugStringA(oss_bones_final.str().c_str());

    OutputDebugStringA("\nModelComponent: Assimp Node Hierarchy (bones identified at load time):\n");
    LogAssimpNodeHierarchy(pScene->mRootNode, m_BoneInfoMap); // Note: m_BoneInfoMap for ID lookup here
    OutputDebugStringA("--- End Assimp Node Hierarchy ---\n\n");
#endif

    oss_constructor.str(""); oss_constructor.clear();
    oss_constructor << "ModelComponent: Finished loading model '" << modelFile << "'\n";
    OutputDebugStringA(oss_constructor.str().c_str());
    Assimp::DefaultLogger::kill(); // Kill logger at the end
}

void ModelComponent::Submit(FrameCommander& frame, Graphics& gfx, dx::FXMMATRIX worldTransform) const noxnd {
    if (pRootInternal) {
        pRootInternal->Submit(frame, gfx, worldTransform);
    }
}

std::unique_ptr<ModelInternalNode> ModelComponent::ParseNodeRecursive(int& nextId, const aiNode& assimpNode, float scale) {
    // Load without initial transpose, store as row-major
    const auto localTransformMatrix = dx::XMLoadFloat4x4(
        reinterpret_cast<const dx::XMFLOAT4X4*>(&assimpNode.mTransformation)
    );

    if (assimpNode.mParent == nullptr) { // Actual root of Assimp scene
        PrintMatrix2("Actual Assimp Root Node Transform (e.g., with_sword/Armature) (Post-Convert)", localTransformMatrix);
    }

    std::vector<Mesh*> currentNodeMeshPtrs;
    currentNodeMeshPtrs.reserve(assimpNode.mNumMeshes);
    for (unsigned int i = 0; i < assimpNode.mNumMeshes; ++i) {
        const auto meshIndex = assimpNode.mMeshes[i];
        if (meshIndex < meshPtrs.size()) {
            currentNodeMeshPtrs.push_back(meshPtrs[meshIndex].get());
        }
    }

    auto pNode = std::make_unique<ModelInternalNode>(nextId++, assimpNode.mName.C_Str(), std::move(currentNodeMeshPtrs), localTransformMatrix);

    for (unsigned int i = 0; i < assimpNode.mNumChildren; ++i) {
        pNode->AddChild(ParseNodeRecursive(nextId, *assimpNode.mChildren[i], scale));
    }
    return pNode;
}


void ModelComponent::ExtractBoneWeightForVertices(
    Dvtx::VertexBuffer& dvtxBuffer,
    const aiMesh& mesh,
    const DirectX::XMMATRIX& meshGlobalInverseTransform // Unused
) {
    if (!dvtxBuffer.GetLayout().Has(Dvtx::VertexLayout::BoneIDs) ||
        !dvtxBuffer.GetLayout().Has(Dvtx::VertexLayout::BoneWeights)) {
        OutputDebugStringA("Warning: ExtractBoneWeightForVertices called for a mesh/material without bone attributes.\n");
        return;
    }

    for (size_t vIdx = 0; vIdx < dvtxBuffer.Size(); ++vIdx) {
        auto& bone_ids = dvtxBuffer[vIdx].Attr<Dvtx::VertexLayout::BoneIDs>();
        auto& bone_weights = dvtxBuffer[vIdx].Attr<Dvtx::VertexLayout::BoneWeights>();
        for (int j = 0; j < MAX_BONES_PER_VERTEX; ++j) {
            bone_ids[j] = 0;
            bone_weights[j] = 0.0f;
        }
    }

    for (unsigned int boneIdx = 0; boneIdx < mesh.mNumBones; ++boneIdx) {
        const aiBone* pAIBone = mesh.mBones[boneIdx];
        std::string boneName = pAIBone->mName.C_Str();
        int currentBoneAssignedID = -1;

        if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end()) {
            BoneInfo newBoneInfo; // Its offsetMatrix is Identity by default (from BoneInfo constructor)
            newBoneInfo.id = m_BoneCounter;
            m_BoneInfoMap[boneName] = newBoneInfo; // Add this new BoneInfo to the map
            currentBoneAssignedID = m_BoneCounter;
            m_BoneCounter++;
        }
        else {
            currentBoneAssignedID = m_BoneInfoMap[boneName].id;
        }

        for (unsigned int weightIdx = 0; weightIdx < pAIBone->mNumWeights; ++weightIdx) {
            const aiVertexWeight& vertexWeight = pAIBone->mWeights[weightIdx];
            unsigned int vertexId = vertexWeight.mVertexId;
            if (vertexId >= dvtxBuffer.Size()) continue;

            auto& bone_ids_for_vertex = dvtxBuffer[vertexId].Attr<Dvtx::VertexLayout::BoneIDs>();
            auto& bone_weights_for_vertex = dvtxBuffer[vertexId].Attr<Dvtx::VertexLayout::BoneWeights>();
            bool slotFound = false;
            for (int slot = 0; slot < MAX_BONES_PER_VERTEX; ++slot) {
                if (bone_weights_for_vertex[slot] < 0.0001f) { // Check for near-zero instead of == 0.0f
                    bone_ids_for_vertex[slot] = currentBoneAssignedID;
                    bone_weights_for_vertex[slot] = vertexWeight.mWeight;
                    slotFound = true;
                    break;
                }
            }
            // if (!slotFound) { /* Warning */ }
        }
    }
    // Normalize weights (same as your previous code)
    for (size_t vIdx = 0; vIdx < dvtxBuffer.Size(); ++vIdx) {
        // ... (your normalization logic) ...
        auto& bone_ids_for_vertex = dvtxBuffer[vIdx].Attr<Dvtx::VertexLayout::BoneIDs>();
        auto& bone_weights_for_vertex = dvtxBuffer[vIdx].Attr<Dvtx::VertexLayout::BoneWeights>();
        float totalWeight = 0.0f;

        for (int j = 0; j < MAX_BONES_PER_VERTEX; ++j) {
            totalWeight += bone_weights_for_vertex[j];
        }

        if (totalWeight > 0.00001f) {
            for (int j = 0; j < MAX_BONES_PER_VERTEX; ++j) {
                bone_weights_for_vertex[j] /= totalWeight;
            }
        }
        else {
            if (mesh.mNumBones > 0 && MAX_BONES_PER_VERTEX > 0) {
                bone_ids_for_vertex[0] = 0;
                bone_weights_for_vertex[0] = 1.0f;
                for (int j = 1; j < MAX_BONES_PER_VERTEX; ++j) {
                    bone_ids_for_vertex[j] = 0;
                    bone_weights_for_vertex[j] = 0.0f;
                }
            }
        }
    }
}

// NEW HELPER FUNCTION
void ModelComponent::CalculateBoneOffsetsRecursive(
    ModelInternalNode* pInternalNode,
    const DirectX::XMMATRIX& parentGlobalBindTransform
) {
    if (!pInternalNode) {
        return;
    }

    std::string nodeName = pInternalNode->GetName();
    DirectX::XMMATRIX localBindTransform = pInternalNode->GetOriginalTransformXM(); // This is node's local transform
    DirectX::XMMATRIX globalBindTransform = localBindTransform * parentGlobalBindTransform;

    auto it = m_BoneInfoMap.find(nodeName);
    if (it != m_BoneInfoMap.end()) {
        // This node is a bone. Its offset matrix is the inverse of its global bind-pose transform.
        DirectX::XMMATRIX offsetMatrixCalculated = DirectX::XMMatrixInverse(nullptr, globalBindTransform);
        DirectX::XMStoreFloat4x4(&(it->second.offsetMatrix), offsetMatrixCalculated); // Update the map entry

        if (nodeName == "hip.L") { // Or any other bone you want to specifically debug
            std::ostringstream oss_mcomponent_store;
            oss_mcomponent_store << "ModelComponent::CalculateBoneOffsetsRecursive - Stored Offset for 'root':\n";
            const DirectX::XMFLOAT4X4& stored_offset = it->second.offsetMatrix;
            oss_mcomponent_store << std::fixed << std::setprecision(3)
                << "[" << stored_offset._11 << ", " << stored_offset._12 << ", " << stored_offset._13 << ", " << stored_offset._14 << "]\n"
                << "[" << stored_offset._21 << ", " << stored_offset._22 << ", " << stored_offset._23 << ", " << stored_offset._24 << "]\n"
                << "[" << stored_offset._31 << ", " << stored_offset._32 << ", " << stored_offset._33 << ", " << stored_offset._34 << "]\n"
                << "[" << stored_offset._41 << ", " << stored_offset._42 << ", " << stored_offset._43 << ", " << stored_offset._44 << "]\n\n";
            OutputDebugStringA(oss_mcomponent_store.str().c_str());

            PrintMatrix2("Offset for root (reloaded in ModelComponent::CalcBoneOffsets)", DirectX::XMLoadFloat4x4(&stored_offset));
        }
    }

    for (const auto& childNodeUPtr : pInternalNode->GetChildren_ForAnim()) {
        if (childNodeUPtr) {
            CalculateBoneOffsetsRecursive(childNodeUPtr.get(), globalBindTransform);
        }
    }
}

void ModelComponent::ShowWindow(Graphics& gfx, const char* windowName) noexcept {
    if (pRootInternal && pControlWindow) {
        pControlWindow->Show(windowName, *pRootInternal, pControlWindow->pSelectedNode, pControlWindow->transforms);
    }
}
// Definition for LogAssimpNodeHierarchy (if not already present or included)
void LogAssimpNodeHierarchy(const aiNode* pNode, const std::unordered_map<std::string, BoneInfo>& boneInfoMap, int indentLevel) {
    if (!pNode) return;
    std::ostringstream oss;
    for (int i = 0; i < indentLevel; ++i) oss << "  ";
    oss << "- Node: '" << pNode->mName.C_Str() << "'";
    auto iter = boneInfoMap.find(pNode->mName.C_Str());
    if (iter != boneInfoMap.end()) {
        oss << " (BONE ID: " << iter->second.id << ")";
    }
    oss << "\n";
    OutputDebugStringA(oss.str().c_str());
    for (unsigned int i = 0; i < pNode->mNumChildren; ++i) {
        LogAssimpNodeHierarchy(pNode->mChildren[i], boneInfoMap, indentLevel + 1);
    }
}