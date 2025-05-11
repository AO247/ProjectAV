#pragma once
#include "Component.h"
#include <vector>
#include <memory>
#include <string>
#include <DirectXMath.h>
#include "ConditionalNoexcept.h"
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
private:
    void AddChild(std::unique_ptr<ModelInternalNode> pChild) noxnd;
private:
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
    virtual ~ModelComponent() noexcept;

    // **** CHANGED Draw to Submit ****
    void Submit(FrameCommander& frame, Graphics& gfx, DirectX::FXMMATRIX worldTransform) const noxnd;

    void ShowWindow(Graphics& gfx, const char* windowName = nullptr) noexcept; // Pass Gfx if ImGui needs it

private:
    // ParseNodeRecursive no longer needs to create Mesh objects, just references them
    std::unique_ptr<ModelInternalNode> ParseNodeRecursive(int& nextId, const aiNode& node, float scale); // Pass scale
    // ParseMesh is removed, Material and Mesh constructor handle this

    // --- Data ---
    std::unique_ptr<ModelInternalNode> pRootInternal;
    std::vector<std::unique_ptr<Mesh>> meshPtrs; // Owns all the Mesh objects
    // std::vector<Material> materials; // No longer needed here if Mesh handles it

    std::unique_ptr<ModelControlWindow> pControlWindow;
};