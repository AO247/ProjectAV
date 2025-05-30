#pragma once
#include "Component.h"
#include <vector>
#include <memory>
#include <string>
#include <DirectXMath.h>
#include "ConditionalNoexcept.h"
// #include "AggregateVertex.h" // No longer needed

// Include DirectX Simple Math
#include <SimpleMath.h>


// Forward Declarations
class Mesh;
class Graphics;
class FrameCommander;
class ModelControlWindow;
namespace Assimp { class Importer; }
struct aiNode;
struct aiMesh;
struct aiMaterial;

struct XMFLOAT3Less {
    bool operator()(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b) const {
        // Lexicographical comparison
        if (a.x != b.x) return a.x < b.x;
        if (a.y != b.y) return a.y < b.y;
        return a.z < b.z;
        // Alternative using std::tie (cleaner, include <tuple>):
        // return std::tie(a.x, a.y, a.z) < std::tie(b.x, b.y, b.z);
    }
};


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
    DirectX::XMFLOAT4X4 transform;
    DirectX::XMFLOAT4X4 appliedTransform;
};


class ModelComponent : public Component
{
public:

    struct Triangle
    {
        DirectX::SimpleMath::Vector3 v0;
        DirectX::SimpleMath::Vector3 v1;
        DirectX::SimpleMath::Vector3 v2;

        Triangle(const DirectX::SimpleMath::Vector3& p0,
            const DirectX::SimpleMath::Vector3& p1,
            const DirectX::SimpleMath::Vector3& p2)
            : v0(p0), v1(p1), v2(p2) {
        }
    };

    ModelComponent(Node* owner, Graphics& gfx, const std::string& modelFile, float scale = 1.0f);
    virtual ~ModelComponent() = default;

    void Submit(FrameCommander& frame, Graphics& gfx, DirectX::FXMMATRIX worldTransform) const noxnd;
    void ShowWindow(Graphics& gfx, const char* windowName = nullptr) noexcept;

    // **** MODIFIED METHOD DECLARATION ****
    std::vector<DirectX::SimpleMath::Vector3> GetAllUniqueVertices() const;
    std::vector<ModelComponent::Triangle> GetAllTriangles() const;

private:
    std::unique_ptr<ModelInternalNode> ParseNodeRecursive(int& nextId, const aiNode& node, float scale);

    std::unique_ptr<ModelInternalNode> pRootInternal;
    std::vector<std::unique_ptr<Mesh>> meshPtrs;
    std::unique_ptr<ModelControlWindow> pControlWindow;
};