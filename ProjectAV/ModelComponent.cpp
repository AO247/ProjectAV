#include "ModelComponent.h"
#include "Mesh.h" 
#include "Node.h" 
#include "Graphics.h"
#include "Material.h" 
#include "FrameCommander.h" 
#include "BindableCommon.h" // For Bind::VertexBuffer etc. (if needed directly)

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "imgui/imgui.h"
#include <unordered_map> // For ModelControlWindow's transforms
#include <sstream>
#include <filesystem>
#include <stdexcept> // For std::runtime_error

namespace dx = DirectX;

namespace Anim
{
    struct BoneInfo {
        int id = -1; // ID of the bone in the final bone matrix array
        DirectX::XMFLOAT4X4 offsetMatrix; // AKA Inverse Bind Pose Matrix
        DirectX::XMFLOAT4X4 nodeTransform; // Store the node's original transform (bind pose local)
        // Could also store parent index, name, etc., for a full skeleton class
    };

    struct KeyPosition {
        DirectX::XMFLOAT3 value;
        float time;
        DirectX::XMMATRIX Interpolate(const std::vector<KeyPosition>& keys, float time, float duration);
    };
    struct KeyRotation {
        DirectX::XMFLOAT4 value; // Quaternion
        float time;
    };
    struct KeyScale {
        DirectX::XMFLOAT3 value;
        float time;
    };

    struct BoneAnimChannel {
        std::string nodeName; // Name of the node/bone affected by this channel
        std::vector<KeyPosition> positionKeys;
        std::vector<KeyRotation> rotationKeys;
        std::vector<KeyScale> scaleKeys;

        // Helper functions to find keyframes (simplified)
        DirectX::XMMATRIX InterpolatePosition(float time) const;
        DirectX::XMMATRIX InterpolateRotation(float time) const; // Returns rotation matrix
        DirectX::XMMATRIX InterpolateScale(float time) const;
    };

    struct AnimationClip {
        std::string name;
        float duration = 0.0f; // Duration in ticks
        float ticksPerSecond = 0.0f;
        std::vector<BoneAnimChannel> channels;
        const BoneAnimChannel* FindChannel(const std::string& nodeName) const;
    };
} // namespace Anim


namespace Anim {
    DirectX::XMMATRIX Anim::KeyPosition::Interpolate(const std::vector<Anim::KeyPosition>& keys, float time, float duration) {
        if (keys.empty()) return DirectX::XMMatrixIdentity();
        if (keys.size() == 1 || time <= keys.front().time) return DirectX::XMMatrixTranslation(keys.front().value.x, keys.front().value.y, keys.front().value.z);
        if (time >= keys.back().time) return DirectX::XMMatrixTranslation(keys.back().value.x, keys.back().value.y, keys.back().value.z);

        size_t i = 0;
        while (i < keys.size() - 1 && time >= keys[i + 1].time) {
            i++;
        }

        const auto& key1 = keys[i];
        const auto& key2 = keys[i + 1];
        float dt = key2.time - key1.time;
        float factor = (dt == 0.0f) ? 0.0f : (time - key1.time) / dt;

        DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&key1.value);
        DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&key2.value);
        DirectX::XMVECTOR result = DirectX::XMVectorLerp(v1, v2, factor);
        return DirectX::XMMatrixTranslationFromVector(result);
    }

    DirectX::XMMATRIX BoneAnimChannel::InterpolatePosition(float time) const {
        if (positionKeys.empty()) return DirectX::XMMatrixIdentity();
        // Find the two keys to interpolate between
        // (Simplified: assumes keys are sorted and time is within duration)
        if (time <= positionKeys.front().time)
            return DirectX::XMMatrixTranslation(positionKeys.front().value.x, positionKeys.front().value.y, positionKeys.front().value.z);
        if (time >= positionKeys.back().time)
            return DirectX::XMMatrixTranslation(positionKeys.back().value.x, positionKeys.back().value.y, positionKeys.back().value.z);

        size_t K0 = 0;
        for (size_t i = 0; i < positionKeys.size() - 1; ++i) {
            if (time < positionKeys[i + 1].time) {
                K0 = i;
                break;
            }
        }
        size_t K1 = K0 + 1;

        float t0 = positionKeys[K0].time;
        float t1 = positionKeys[K1].time;
        float dt = t1 - t0;
        float factor = (dt == 0.0f) ? 0.0f : (time - t0) / dt;

        DirectX::XMVECTOR v0 = DirectX::XMLoadFloat3(&positionKeys[K0].value);
        DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&positionKeys[K1].value);
        return DirectX::XMMatrixTranslationFromVector(DirectX::XMVectorLerp(v0, v1, factor));
    }

    DirectX::XMMATRIX Anim::BoneAnimChannel::InterpolateRotation(float time) const {
        if (rotationKeys.empty()) return DirectX::XMMatrixIdentity();
        if (time <= rotationKeys.front().time)
            return DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&rotationKeys.front().value));
        if (time >= rotationKeys.back().time)
            return DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&rotationKeys.back().value));

        size_t K0 = 0;
        for (size_t i = 0; i < rotationKeys.size() - 1; ++i) {
            if (time < rotationKeys[i + 1].time) {
                K0 = i;
                break;
            }
        }
        size_t K1 = K0 + 1;

        float t0 = rotationKeys[K0].time;
        float t1 = rotationKeys[K1].time;
        float dt = t1 - t0;
        float factor = (dt == 0.0f) ? 0.0f : (time - t0) / dt;

        DirectX::XMVECTOR q0 = DirectX::XMLoadFloat4(&rotationKeys[K0].value);
        DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(&rotationKeys[K1].value);
        return DirectX::XMMatrixRotationQuaternion(DirectX::XMQuaternionSlerp(q0, q1, factor));
    }

    DirectX::XMMATRIX Anim::BoneAnimChannel::InterpolateScale(float time) const {
        if (scaleKeys.empty()) return DirectX::XMMatrixIdentity();
        if (time <= scaleKeys.front().time)
            return DirectX::XMMatrixScaling(scaleKeys.front().value.x, scaleKeys.front().value.y, scaleKeys.front().value.z);
        if (time >= scaleKeys.back().time)
            return DirectX::XMMatrixScaling(scaleKeys.back().value.x, scaleKeys.back().value.y, scaleKeys.back().value.z);

        size_t K0 = 0;
        for (size_t i = 0; i < scaleKeys.size() - 1; ++i) {
            if (time < scaleKeys[i + 1].time) {
                K0 = i;
                break;
            }
        }
        size_t K1 = K0 + 1;

        float t0 = scaleKeys[K0].time;
        float t1 = scaleKeys[K1].time;
        float dt = t1 - t0;
        float factor = (dt == 0.0f) ? 0.0f : (time - t0) / dt;

        DirectX::XMVECTOR s0 = DirectX::XMLoadFloat3(&scaleKeys[K0].value);
        DirectX::XMVECTOR s1 = DirectX::XMLoadFloat3(&scaleKeys[K1].value);
        return DirectX::XMMatrixScalingFromVector(DirectX::XMVectorLerp(s0, s1, factor));
    }


    const Anim::BoneAnimChannel* Anim::AnimationClip::FindChannel(const std::string& nodeName) const {
        for (const auto& channel : channels) {
            if (channel.nodeName == nodeName) {
                return &channel;
            }
        }
        return nullptr;
    }
}


// --- TransformParameters, ModelControlWindow, ModelInternalNode implementations remain the same ---
// (Ensure these are fully defined as in the original provided code)
struct TransformParameters
{
    float roll = 0.0f;
    float pitch = 0.0f;
    float yaw = 0.0f;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

class ModelControlWindow
{
public:
    void Show(const char* windowName, const ModelInternalNode& root, ModelInternalNode*& pSelectedNode,
        std::unordered_map<int, TransformParameters>& transforms) noexcept
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
                auto& transform = transforms[selectedId];
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

                pSelectedNode->SetAppliedTransform(
                    dx::XMMatrixRotationRollPitchYaw(transform.pitch, transform.yaw, transform.roll) *
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
    ModelInternalNode* pSelectedNode = nullptr;
    std::unordered_map<int, TransformParameters> transforms;
};

ModelInternalNode::ModelInternalNode(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const dx::XMMATRIX& transform_in) noxnd
    : id(id), name(name), meshPtrs(std::move(meshPtrs))
{
    dx::XMStoreFloat4x4(&transform, transform_in);
    dx::XMStoreFloat4x4(&appliedTransform, dx::XMMatrixIdentity());
}

void ModelInternalNode::AddChild(std::unique_ptr<ModelInternalNode> pChild) noxnd
{
    assert(pChild);
    childPtrs.push_back(std::move(pChild));
}

void ModelInternalNode::Submit(FrameCommander& frame, Graphics& gfx, dx::FXMMATRIX accumulatedTransform) const noxnd
{
    const auto modelNodeTransform =
        dx::XMLoadFloat4x4(&appliedTransform) *
        dx::XMLoadFloat4x4(&transform) *
        accumulatedTransform;

    for (auto pm : meshPtrs)
    {
        pm->Submit(frame, modelNodeTransform);
    }
    for (const auto& pc : childPtrs)
    {
        pc->Submit(frame, gfx, modelNodeTransform);
    }
}
int ModelInternalNode::GetId() const noexcept { return id; }
void ModelInternalNode::SetAppliedTransform(DirectX::FXMMATRIX transform_in) noexcept { dx::XMStoreFloat4x4(&appliedTransform, transform_in); }
void ModelInternalNode::ShowTree(int& nodeIndexTracker, ModelInternalNode*& pSelectedNode_ref) const noexcept
{
    nodeIndexTracker++;
    const int currentId = GetId();
    const bool isSelected = (pSelectedNode_ref != nullptr) && (pSelectedNode_ref->GetId() == currentId);
    const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow |
        (isSelected ? ImGuiTreeNodeFlags_Selected : 0) |
        (childPtrs.empty() ? ImGuiTreeNodeFlags_Leaf : 0);

    const bool expanded = ImGui::TreeNodeEx(
        (void*)(intptr_t)(currentId + nodeIndexTracker * 100000),
        node_flags, name.c_str()
    );
    if (ImGui::IsItemClicked())
    {
        pSelectedNode_ref = const_cast<ModelInternalNode*>(this);
    }
    if (expanded)
    {
        for (const auto& pChild : childPtrs)
        {
            pChild->ShowTree(nodeIndexTracker, pSelectedNode_ref);
        }
        ImGui::TreePop();
    }
}


// --- ModelComponent Static Helpers ---
DirectX::XMMATRIX ModelComponent::AiToDxm(const aiMatrix4x4& aiMat) {
    // Assimp matrices are row-major, DirectX math typically prefers column-major for mul(matrix, vector) in HLSL
    // or if its matrix multiplication order implies it. XMStoreFloat4x4 stores in row-major order.
    // The XMMatrixTranspose in ParseNodeRecursive suggests a convention.
    // For consistency, we'll transpose.
    return DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(&aiMat)));
}
DirectX::XMFLOAT3 ModelComponent::AiToDxf3(const aiVector3D& aiVec) {
    return { aiVec.x, aiVec.y, aiVec.z };
}
DirectX::XMFLOAT4 ModelComponent::AiToDxf4(const aiQuaternion& aiQuat) {
    return { aiQuat.x, aiQuat.y, aiQuat.z, aiQuat.w };
}

// --- ModelComponent Implementation ---
ModelComponent::ModelComponent(Node* owner, Graphics& gfx, const std::string& modelFile, float scale)
    : Component(owner),
    m_pImporter(std::make_unique<Assimp::Importer>()),
    pControlWindow(std::make_unique<ModelControlWindow>())
{
    m_pScene = m_pImporter->ReadFile(modelFile.c_str(),
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_ConvertToLeftHanded | // Ensure coordinate system consistency
        aiProcess_GenNormals |          // Generate normals if not present
        aiProcess_CalcTangentSpace |    // Generate tangents and bitangents
        aiProcess_LimitBoneWeights      // Ensure max 4 (or configurable) bones per vertex
    );

    if (!m_pScene || !m_pScene->mRootNode || m_pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
        throw ModelException(__LINE__, __FILE__, "Assimp error: " + std::string(m_pImporter->GetErrorString()));
    }

    // Store global inverse transform of the root node
    DirectX::XMStoreFloat4x4(&m_GlobalInverseTransform, DirectX::XMMatrixInverse(nullptr, AiToDxm(m_pScene->mRootNode->mTransformation)));

    ProcessBones(m_pScene); // Populate m_BoneInfoMap and m_BoneCounter
    ParseAnimations(m_pScene); // Populate m_Animations

    if (IsSkinned()) {
        InitializeBoneCbuf(gfx); // Create the constant buffer for bone matrices
    }

    std::filesystem::path filePath(modelFile);
    std::vector<Material> materials;
    materials.reserve(m_pScene->mNumMaterials);
    for (size_t i = 0; i < m_pScene->mNumMaterials; ++i) {
        bool isUsedBySkinnedMesh = false;
        for (unsigned int k = 0; k < m_pScene->mNumMeshes; ++k) {
            if (m_pScene->mMeshes[k]->mMaterialIndex == i && m_pScene->mMeshes[k]->HasBones()) {
                isUsedBySkinnedMesh = true;
                break;
            }
        }
        materials.emplace_back(gfx, *m_pScene->mMaterials[i], filePath, isUsedBySkinnedMesh);
    }

    meshPtrs.reserve(m_pScene->mNumMeshes);
    for (unsigned int i = 0; i < m_pScene->mNumMeshes; ++i) {
        const auto& assimpMesh = *m_pScene->mMeshes[i];

        std::vector<Dvtx::VertexBoneData> vertexBoneDataForThisMesh;
        if (assimpMesh.HasBones()) {
            ExtractBoneWeightsForMesh(vertexBoneDataForThisMesh, assimpMesh);
        }

        meshPtrs.push_back(std::make_unique<Mesh>(
            gfx,
            materials[assimpMesh.mMaterialIndex],
            assimpMesh,
            scale,
            vertexBoneDataForThisMesh // Pass bone data
        ));
    }

    int nextId = 0;
    pRootInternal = ParseNodeRecursive(nextId, *m_pScene->mRootNode, scale);

    if (HasAnimations()) {
        SetAnimation(0); // Default to first animation
    }
    else if (IsSkinned()) {
        // If skinned but no animations, prepare for bind pose
        m_FinalBoneMatrices.assign(m_BoneCounter, DirectX::XMFLOAT4X4{}); // Zero initialize
        for (int i = 0; i < m_BoneCounter; ++i) DirectX::XMStoreFloat4x4(&m_FinalBoneMatrices[i], DirectX::XMMatrixIdentity());
    }
}

ModelComponent::~ModelComponent() noexcept = default;

void ModelComponent::Submit(FrameCommander& frame, Graphics& gfx, dx::FXMMATRIX worldTransform) const noxnd
{
    if (IsSkinned() && m_pBoneCbuf) {
        // Update and bind bone CBuf
        D3D11_MAPPED_SUBRESOURCE msr;
        gfx.GetContext()->Map(m_pBoneCbuf.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr);
        memcpy(msr.pData, m_FinalBoneMatrices.data(), m_FinalBoneMatrices.size() * sizeof(DirectX::XMFLOAT4X4));
        gfx.GetContext()->Unmap(m_pBoneCbuf.Get(), 0u);
        gfx.GetContext()->VSSetConstantBuffers(m_BoneCbufSlot, 1u, m_pBoneCbuf.GetAddressOf());
    }

    if (pRootInternal) {
        pRootInternal->Submit(frame, gfx, worldTransform);
    }
}

std::unique_ptr<ModelInternalNode> ModelComponent::ParseNodeRecursive(int& nextId, const aiNode& node, float scale)
{
    const auto transformMatrix = AiToDxm(node.mTransformation); // Already transposed by AiToDxm
    // Scaling is typically applied at vertex level or baked into bind pose.
    // If scale needs to apply to node transforms:
    // dx::XMMATRIX finalNodeTransform = dx::XMMatrixScaling(scale, scale, scale) * transformMatrix; (if scale is uniform)
    // Or, decompose, scale translation, recompose. For now, assume scale is handled by Mesh.
    dx::XMMATRIX finalNodeTransform = transformMatrix;

    std::vector<Mesh*> currentNodeMeshPtrs;
    currentNodeMeshPtrs.reserve(node.mNumMeshes);
    for (unsigned int i = 0; i < node.mNumMeshes; ++i) {
        const auto meshIndex = node.mMeshes[i];
        if (meshIndex < meshPtrs.size()) {
            currentNodeMeshPtrs.push_back(meshPtrs[meshIndex].get());
        }
        else {
            // Should not happen if meshPtrs is populated correctly
            OutputDebugStringA(("Warning: Mesh index out of bounds in node " + std::string(node.mName.C_Str()) + "\n").c_str());
        }
    }

    auto pNode = std::make_unique<ModelInternalNode>(nextId++, node.mName.C_Str(), std::move(currentNodeMeshPtrs), finalNodeTransform);

    for (unsigned int i = 0; i < node.mNumChildren; ++i) {
        pNode->AddChild(ParseNodeRecursive(nextId, *node.mChildren[i], scale));
    }
    return pNode;
}

void ModelComponent::ShowWindow(Graphics& gfx, const char* windowName) noexcept
{
    if (pRootInternal && pControlWindow) {
        pControlWindow->Show(windowName, *pRootInternal, pControlWindow->pSelectedNode, pControlWindow->transforms);

        // Add animation controls to ImGui window
        if (ImGui::Begin(windowName ? windowName : "Model Controls")) { // Re-open if closed by pControlWindow->Show
            if (HasAnimations()) {
                ImGui::Separator();
                ImGui::Text("Animations");
                if (ImGui::BeginCombo("Clip", m_Animations[m_CurrentAnimationIndex].name.c_str())) {
                    for (int i = 0; i < m_Animations.size(); ++i) {
                        const bool is_selected = (m_CurrentAnimationIndex == i);
                        if (ImGui::Selectable(m_Animations[i].name.c_str(), is_selected)) {
                            SetAnimation(i);
                        }
                        if (is_selected) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                ImGui::SliderFloat("Time", &m_AnimationTime, 0.0f, m_Animations[m_CurrentAnimationIndex].duration);
            }
        }
        ImGui::End(); // End for the Begin in this ShowWindow if it was used
    }
}

void ModelComponent::InitializeBoneCbuf(Graphics& gfx) {
    // Max bones shader supports (ensure this matches shader)
    // For dynamic sizing, you might resize m_FinalBoneMatrices and re-create cbuf if m_BoneCounter changes,
    // but usually, models have a fixed bone count.
    // Let's use a fixed max size for the CBuffer for simplicity.
    const UINT maxShaderBones = 128; // Example, should match shader
    m_FinalBoneMatrices.resize(maxShaderBones);
    // Fill with identity initially if not already done by SetAnimation or constructor
    for (auto& mat : m_FinalBoneMatrices) DirectX::XMStoreFloat4x4(&mat, DirectX::XMMatrixIdentity());


    D3D11_BUFFER_DESC cbd = {};
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cbd.MiscFlags = 0u;
    cbd.ByteWidth = sizeof(DirectX::XMFLOAT4X4) * maxShaderBones; // Max size
    cbd.StructureByteStride = 0u;

    HRESULT hr = gfx.GetDevice()->CreateBuffer(&cbd, nullptr, &m_pBoneCbuf);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create bone constant buffer.");
    }
}


// --- Animation and Bone Processing Methods ---
void ModelComponent::ProcessBones(const aiScene* pSceneToProcess) {
    m_BoneCounter = 0; // Reset
    m_BoneInfoMap.clear();

    for (unsigned int meshIdx = 0; meshIdx < pSceneToProcess->mNumMeshes; ++meshIdx) {
        const aiMesh* pAiMesh = pSceneToProcess->mMeshes[meshIdx];
        for (unsigned int boneIdx = 0; boneIdx < pAiMesh->mNumBones; ++boneIdx) {
            const aiBone* pAiBone = pAiMesh->mBones[boneIdx];
            std::string boneName = pAiBone->mName.C_Str();

            if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end()) {
                Anim::BoneInfo newBoneInfo;
                newBoneInfo.id = m_BoneCounter++;
                // Assimp's offset matrix transforms from mesh space to bone space (it's the inverse of bind pose transform)
                // Transpose it for DirectXMath conventions if needed by AiToDxm
                DirectX::XMStoreFloat4x4(&newBoneInfo.offsetMatrix, AiToDxm(pAiBone->mOffsetMatrix));
                m_BoneInfoMap[boneName] = newBoneInfo;
            }
        }
    }
    // Resize based on actual bones found, or use a max size for shader.
    // The CBuffer will be sized to maxShaderBones, m_FinalBoneMatrices too.
    // m_FinalBoneMatrices.resize(m_BoneCounter); // If using exact size
}

void ModelComponent::ExtractBoneWeightsForMesh(std::vector<Dvtx::VertexBoneData>& outVertexBoneData, const aiMesh& mesh) const {
    outVertexBoneData.assign(mesh.mNumVertices, Dvtx::VertexBoneData{}); // Initialize for all vertices

    for (unsigned int boneIdx = 0; boneIdx < mesh.mNumBones; ++boneIdx) {
        const aiBone* pAiBone = mesh.mBones[boneIdx];
        std::string boneName = pAiBone->mName.C_Str();

        auto it = m_BoneInfoMap.find(boneName);
        if (it == m_BoneInfoMap.end()) {
            // This bone was not found in the map, might be an issue or an unreferenced bone.
            // OutputDebugStringA(("Warning: Bone " + boneName + " from mesh not found in global bone map.\n").c_str());
            continue;
        }
        int boneID = it->second.id;

        for (unsigned int weightIdx = 0; weightIdx < pAiBone->mNumWeights; ++weightIdx) {
            unsigned int vertexId = pAiBone->mWeights[weightIdx].mVertexId;
            float weight = pAiBone->mWeights[weightIdx].mWeight;
            if (vertexId < outVertexBoneData.size()) {
                outVertexBoneData[vertexId].Add(boneID, weight);
            }
        }
    }

    // Normalize weights for each vertex
    for (auto& vbd : outVertexBoneData) {
        vbd.Normalize();
    }
}

void ModelComponent::ParseAnimations(const aiScene* pSceneToParse) {
    m_Animations.clear();
    for (unsigned int i = 0; i < pSceneToParse->mNumAnimations; ++i) {
        const aiAnimation* pAiAnimation = pSceneToParse->mAnimations[i];
        Anim::AnimationClip clip;
        clip.name = pAiAnimation->mName.C_Str();
        if (clip.name.empty()) clip.name = "Anim_" + std::to_string(i);
        clip.duration = static_cast<float>(pAiAnimation->mDuration);
        clip.ticksPerSecond = static_cast<float>(pAiAnimation->mTicksPerSecond > 0.0 ? pAiAnimation->mTicksPerSecond : 25.0f); // Default TPS

        for (unsigned int j = 0; j < pAiAnimation->mNumChannels; ++j) {
            const aiNodeAnim* pAiNodeAnim = pAiAnimation->mChannels[j];
            Anim::BoneAnimChannel channel;
            channel.nodeName = pAiNodeAnim->mNodeName.C_Str();

            for (unsigned int k = 0; k < pAiNodeAnim->mNumPositionKeys; ++k) {
                channel.positionKeys.push_back({ AiToDxf3(pAiNodeAnim->mPositionKeys[k].mValue), (float)pAiNodeAnim->mPositionKeys[k].mTime });
            }
            for (unsigned int k = 0; k < pAiNodeAnim->mNumRotationKeys; ++k) {
                channel.rotationKeys.push_back({ AiToDxf4(pAiNodeAnim->mRotationKeys[k].mValue), (float)pAiNodeAnim->mRotationKeys[k].mTime });
            }
            for (unsigned int k = 0; k < pAiNodeAnim->mNumScalingKeys; ++k) { // Often, scaling is uniform or baked.
                channel.scaleKeys.push_back({ AiToDxf3(pAiNodeAnim->mScalingKeys[k].mValue), (float)pAiNodeAnim->mScalingKeys[k].mTime });
            }
            clip.channels.push_back(std::move(channel));
        }
        m_Animations.push_back(std::move(clip));
    }
}

void ModelComponent::SetAnimation(int animIndex) {
    if (animIndex < 0 || animIndex >= m_Animations.size()) {
        m_CurrentAnimationIndex = -1; // No animation / bind pose
        m_AnimationTime = 0.0f;
        // For bind pose, final bone matrices should effectively be identity if shader multiplies by offset matrix.
        // Or, they are OffsetMatrix * GlobalTransformOfBoneInBindPose.
        // If shader computes: V_skinned = sum( (V_bind * OffsetMatrix_i * GlobalAnimatedTransform_i) * weight_i )
        // For bind pose, GlobalAnimatedTransform_i is the inverse of OffsetMatrix_i (relative to model space).
        // So (OffsetMatrix_i * InverseOffsetMatrix_i) = Identity.
        // Thus, for bind pose, m_FinalBoneMatrices should be identity.
        if (IsSkinned()) {
            for (size_t i = 0; i < m_FinalBoneMatrices.size(); ++i) { // Use .size() for safety, up to maxShaderBones
                if (i < m_BoneCounter) // Only for actual bones
                    DirectX::XMStoreFloat4x4(&m_FinalBoneMatrices[i], DirectX::XMMatrixIdentity());
                else // For unused slots in CBuffer, also identity
                    DirectX::XMStoreFloat4x4(&m_FinalBoneMatrices[i], DirectX::XMMatrixIdentity());

            }
        }
    }
    else {
        m_CurrentAnimationIndex = animIndex;
        m_AnimationTime = 0.0f;
    }
    // Initial update for the new animation state
    UpdateAnimation(0.0f);
}

void ModelComponent::UpdateAnimation(float dt) noxnd {
    if (m_CurrentAnimationIndex == -1 || m_Animations.empty() || !IsSkinned() || !m_pScene) {
        // If no animation or not skinned, ensure bind pose (already handled by SetAnimation(-1) or initial state)
        // Or if called before scene fully loaded.
        return;
    }

    const Anim::AnimationClip& currentClip = m_Animations[m_CurrentAnimationIndex];
    m_AnimationTime += dt * currentClip.ticksPerSecond;
    if (m_AnimationTime > currentClip.duration) {
        m_AnimationTime = fmod(m_AnimationTime, currentClip.duration); // Loop animation
    }

    // Recursively calculate bone transforms
    CalculateBoneTransformsRecursive(m_pScene->mRootNode, m_AnimationTime, &m_Animations[m_CurrentAnimationIndex], DirectX::XMMatrixIdentity());
}

void ModelComponent::CalculateBoneTransformsRecursive(const aiNode* pNode, float animationTime, const Anim::AnimationClip* pCurrentAnim, DirectX::CXMMATRIX parentTransform) {
    std::string nodeName = pNode->mName.C_Str();
    DirectX::XMMATRIX nodeTransform = AiToDxm(pNode->mTransformation); // Node's bind pose transform relative to parent

    if (pCurrentAnim) {
        const Anim::BoneAnimChannel* pChannel = pCurrentAnim->FindChannel(nodeName);
        if (pChannel) {
            DirectX::XMMATRIX S = pChannel->InterpolateScale(animationTime);
            DirectX::XMMATRIX R = pChannel->InterpolateRotation(animationTime);
            DirectX::XMMATRIX T = pChannel->InterpolatePosition(animationTime);
            nodeTransform = S * R * T; // SRT order is common
        }
    }

    DirectX::XMMATRIX globalTransform = nodeTransform * parentTransform;

    auto boneInfoIt = m_BoneInfoMap.find(nodeName);
    if (boneInfoIt != m_BoneInfoMap.end()) {
        const Anim::BoneInfo& boneInfo = boneInfoIt->second;
        if (boneInfo.id < m_FinalBoneMatrices.size()) { // Check against maxShaderBones size
            // Final matrix for GPU = GlobalInverseRootTransform * BoneGlobalAnimatedTransform * BoneOffsetMatrix
            // BoneOffsetMatrix is Inverse Bind Pose.
            // So, this brings vertex from its local bind pose, to bone's animated pose, then to model space.
            DirectX::XMMATRIX offsetMatrix = DirectX::XMLoadFloat4x4(&boneInfo.offsetMatrix);
            DirectX::XMMATRIX globalInverseRoot = DirectX::XMLoadFloat4x4(&m_GlobalInverseTransform);
            DirectX::XMStoreFloat4x4(&m_FinalBoneMatrices[boneInfo.id], offsetMatrix * globalTransform * globalInverseRoot);
            // Simpler if all transforms are relative to a common model root (which AiToDxm + ConvertToLeftHanded might achieve)
            // Then: m_FinalBoneMatrices[boneInfo.id] = offsetMatrix * globalTransform;
            // The exact math depends heavily on coordinate system conventions and how OffsetMatrix is defined.
            // Common form: Final = InverseBindPoseMatrix * GlobalAnimatedBoneTransform
            // Assuming globalTransform IS the GlobalAnimatedBoneTransform relative to model root:
            // DirectX::XMStoreFloat4x4(&m_FinalBoneMatrices[boneInfo.id], offsetMatrix * globalTransform);

        }
    }

    for (unsigned int i = 0; i < pNode->mNumChildren; ++i) {
        CalculateBoneTransformsRecursive(pNode->mChildren[i], animationTime, pCurrentAnim, globalTransform);
    }
}