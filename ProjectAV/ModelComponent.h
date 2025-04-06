#pragma once
#include "Component.h"
#include <vector>
#include <memory>
#include <string>
#include <DirectXMath.h>
#include "ConditionalNoexcept.h" // For noxnd

// Forward Declarations
class Mesh;
class Graphics;
namespace Assimp { class Importer; } // Forward declare instead of including Assimp headers here
struct aiNode;
struct aiMesh;
struct aiMaterial;
namespace Bind { class Bindable; } // Forward declaration

// Use pImpl idiom for ModelWindow to hide ImGui details from the header
class ModelControlWindow;

// This internal Node structure represents the hierarchy loaded FROM the model file
// It is distinct from the main scene Node class.
class ModelInternalNode
{
    friend class ModelComponent; // Allow ModelComponent to access private members/methods
public:
    ModelInternalNode(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noxnd;
    const std::string& GetName() const noexcept { return name; }
    void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noxnd;
    void ShowTree(int& nodeIndexTracker, ModelInternalNode*& pSelectedNode) const noexcept; // Pass selected node state
    void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept; // Used by ImGui controls
    int GetId() const noexcept;

private:
    void AddChild(std::unique_ptr<ModelInternalNode> pChild) noxnd;

private:
    std::string name;

    int id;
    std::vector<std::unique_ptr<ModelInternalNode>> childPtrs;
    std::vector<Mesh*> meshPtrs; // Pointers to meshes owned by ModelComponent
    DirectX::XMFLOAT4X4 transform; // Local transform within the model file's hierarchy
    DirectX::XMFLOAT4X4 appliedTransform; // Additional transform applied via ImGui
};


class ModelComponent : public Component
{
public:
    ModelComponent(Node* owner, Graphics& gfx, const std::string& modelFile);
    virtual ~ModelComponent() noexcept; // Need destructor to manage pImpl

    // Draw the model using the provided world transform from the owning Node
    void Draw(Graphics& gfx, DirectX::FXMMATRIX worldTransform) const noxnd;

    // Show ImGui control window
    void ShowWindow(const char* windowName = nullptr) noexcept;


private:
    // --- Assimp Loading ---
    std::unique_ptr<ModelInternalNode> ParseNodeRecursive(int& nextId, const aiNode& node) noexcept;
    std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::string& modelPath);


    // --- Data ---
    std::unique_ptr<ModelInternalNode> pRootInternal; // Root of the *internal* hierarchy loaded from file
    std::vector<std::unique_ptr<Mesh>> meshPtrs; // Owns all the meshes for this model instance

    // --- ImGui Window --- (Using pImpl)
    std::unique_ptr<ModelControlWindow> pControlWindow;
};