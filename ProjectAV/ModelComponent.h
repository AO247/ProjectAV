#pragma once
#include "Component.h"
#include <vector>
#include <memory>
#include <string>
#include <map> // For bone map
#include <DirectXMath.h>
#include "ConditionalNoexcept.h"
#include "Vertex.h" // For Dvtx::VertexBoneData

// Forward Declarations
class Mesh;
class Graphics;
class FrameCommander;
class ModelControlWindow;
namespace Assimp { class Importer; class Scene; } // Added Assimp::Scene
struct aiNode;
struct aiMesh;
struct aiMaterial;
struct aiAnimation;
struct aiNodeAnim;

// Animation-related structures (minimal version)
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


class ModelInternalNode
{
    friend class ModelComponent;
public:
    ModelInternalNode(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noxnd;
    const std::string& GetName() const noexcept { return name; }
    void Submit(FrameCommander& frame, Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noxnd;
    void ShowTree(int& nodeIndexTracker, ModelInternalNode*& pSelectedNode) const noexcept;
    void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
    int GetId() const noexcept;
private:
    void AddChild(std::unique_ptr<ModelInternalNode> pChild) noxnd;
private:
    std::string name;
    int id;
    std::vector<std::unique_ptr<ModelInternalNode>> childPtrs;
    std::vector<Mesh*> meshPtrs;
    DirectX::XMFLOAT4X4 transform; // Original transform from file (relative to parent)
    DirectX::XMFLOAT4X4 appliedTransform; // ImGui applied transform (relative to original)
};


class ModelComponent : public Component
{
public:
    ModelComponent(Node* owner, Graphics& gfx, const std::string& modelFile, float scale = 1.0f);
    virtual ~ModelComponent() noexcept;

    void Submit(FrameCommander& frame, Graphics& gfx, DirectX::FXMMATRIX worldTransform) const noxnd;
    void ShowWindow(Graphics& gfx, const char* windowName = nullptr) noexcept;

    void UpdateAnimation(float dt) noxnd; // To be called from game loop
    bool HasAnimations() const { return !m_Animations.empty(); }
    bool IsSkinned() const { return m_BoneCounter > 0; }
    void SetAnimation(int animIndex);
    int GetCurrentAnimationIndex() const { return m_CurrentAnimationIndex; }
    const std::vector<Anim::AnimationClip>& GetAnimations() const { return m_Animations; }


private:
    std::unique_ptr<ModelInternalNode> ParseNodeRecursive(int& nextId, const aiNode& node, float scale);

    // Animation and Bone processing
    void ProcessBones(const aiScene* pScene);
    void ExtractBoneWeightsForMesh(std::vector<Dvtx::VertexBoneData>& outVertexBoneData, const aiMesh& mesh) const;
    void ParseAnimations(const aiScene* pScene);
    void CalculateBoneTransformsRecursive(const aiNode* pNode, float animationTime, const Anim::AnimationClip* pCurrentAnim, DirectX::CXMMATRIX parentTransform);

    // Helper to convert Assimp matrices/vectors to DirectXMath
    static DirectX::XMMATRIX AiToDxm(const aiMatrix4x4& aiMat);
    static DirectX::XMFLOAT3 AiToDxf3(const aiVector3D& aiVec);
    static DirectX::XMFLOAT4 AiToDxf4(const aiQuaternion& aiQuat);


    // --- Data ---
    std::unique_ptr<Assimp::Importer> m_pImporter; // Owns the Assimp scene data
    const aiScene* m_pScene = nullptr; // Non-owning pointer to the loaded scene

    std::unique_ptr<ModelInternalNode> pRootInternal;
    std::vector<std::unique_ptr<Mesh>> meshPtrs;

    std::unique_ptr<ModelControlWindow> pControlWindow;

    // Animation Data
    std::map<std::string, Anim::BoneInfo> m_BoneInfoMap; // Maps bone name to its info (ID, offset matrix)
    int m_BoneCounter = 0;
    std::vector<DirectX::XMFLOAT4X4> m_FinalBoneMatrices; // Final transforms for GPU skinning

    std::vector<Anim::AnimationClip> m_Animations;
    int m_CurrentAnimationIndex = -1; // -1 for no animation (bind pose)
    float m_AnimationTime = 0.0f;
    DirectX::XMFLOAT4X4 m_GlobalInverseTransform; // Root node's inverse transform

    // DirectX Resources for Bone CBuffer (managed by ModelComponent)
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_pBoneCbuf;
    UINT m_BoneCbufSlot = 1u; // Default VS slot for bone matrices CBuffer
    void InitializeBoneCbuf(Graphics& gfx);
};