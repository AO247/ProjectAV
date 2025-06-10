#pragma once
#include "Vertex.h"
#include "Component.h"
#include <vector>
#include <memory>
#include <string>
#include <DirectXMath.h>
#include "ConditionalNoexcept.h"
#include "Bone.h"
#include <unordered_map>

// Forward Declarations
class Mesh; // Your Mesh class
class Graphics;
class FrameCommander; // For Submit method
class ModelControlWindow; // Keep for ImGui
namespace Assimp { class Importer; }
struct aiNode; // Assimp internal node
struct aiMesh;
struct aiMaterial;

// This internal Node structure (ModelInternalNode) represents the hierarchy loaded FROM the model file
class ModelInternalNode
{
    friend class ModelComponent;
public:
    ModelInternalNode(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noxnd;
    const std::string& GetName() const noexcept { return name; }
    // **** CHANGED Draw to Submit ****
    void Submit(FrameCommander& frame, Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noxnd;
    void ShowTree(int& nodeIndexTracker, ModelInternalNode*& pSelectedNode) const noexcept;
    void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
    int GetId() const noexcept;

    const DirectX::XMFLOAT4X4& GetOriginalTransformMatrixF4X4() const noexcept { return transform; } // Returns the stored XMFLOAT4X4 by const ref
    DirectX::XMMATRIX GetOriginalTransformXM() const noexcept { return DirectX::XMLoadFloat4x4(&transform); } // Returns an XMMATRIX

    const std::vector<std::unique_ptr<ModelInternalNode>>& GetChildren() const noexcept { return childPtrs; }
    const DirectX::XMFLOAT4X4& GetBindPoseTransform() const noexcept { return transform; }
    
    //const std::string& GetName() const noexcept { return name; }

    const DirectX::XMFLOAT4X4& GetOriginalTransform_ForAnim() const { return transform; } // 'transform' stores the node's original local matrix
    const std::vector<std::unique_ptr<ModelInternalNode>>& GetChildren_ForAnim() const { return childPtrs; }
private:
    void AddChild(std::unique_ptr<ModelInternalNode> pChild) noxnd;
    std::string name;
    int id;
    std::vector<std::unique_ptr<ModelInternalNode>> childPtrs;
    std::vector<Mesh*> meshPtrs;
    DirectX::XMFLOAT4X4 transform;
    DirectX::XMFLOAT4X4 appliedTransform;
};


class ModelComponent : public Component
{
public:
    // Constructor will take model file and scale
    ModelComponent(Node* owner, Graphics& gfx, const std::string& modelFile, float scale = 1.0f);
    virtual ~ModelComponent() = default;

    // **** CHANGED Draw to Submit ****
    void Submit(FrameCommander& frame, Graphics& gfx, DirectX::FXMMATRIX worldTransform) const noxnd;

    void ShowWindow(Graphics& gfx, const char* windowName = nullptr) noexcept; // Pass Gfx if ImGui needs it

    int GetBoneCount() const { return m_BoneCounter; }

    ModelInternalNode* GetRootInternalNode() const { return pRootInternal.get(); }
    std::unique_ptr<ModelInternalNode> pRootInternal;
    const std::unordered_map<std::string, BoneInfo>& GetBoneInfoMap() const { return m_BoneInfoMap; }

private:
    // ParseNodeRecursive no longer needs to create Mesh objects, just references them
    std::unique_ptr<ModelInternalNode> ParseNodeRecursive(int& nextId, const aiNode& node, float scale); // Pass scale
    // ParseMesh is removed, Material and Mesh constructor handle this

    // --- Data ---
    
    std::vector<std::unique_ptr<Mesh>> meshPtrs; // Owns all the Mesh objects
    // std::vector<Material> materials; // No longer needed here if Mesh handles it

    std::unique_ptr<ModelControlWindow> pControlWindow;

    std::unordered_map<std::string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;

    void ExtractBoneWeightForVertices(
        Dvtx::VertexBuffer& vertices, // Note: it's Dvtx::VertexBuffer&
        const aiMesh& mesh,
        const DirectX::XMMATRIX& meshGlobalInverseTransform // Or however you named this last parameter
    );

    void CalculateBoneOffsetsRecursive(
        ModelInternalNode* pInternalNode,
        const DirectX::XMMATRIX& parentWorldTransform
        // No need to pass m_BoneInfoMap if it's a member, or pass by ref if you prefer
    );


};