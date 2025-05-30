#pragma once
#include <vector>
#include <memory>
#include <string>
#include <DirectXMath.h>
#include <typeinfo> 
#include "Component.h"
#include <SimpleMath.h> // For Vector3

class Graphics;
class FrameCommander; // Forward declare
class Component;

using namespace DirectX::SimpleMath; // For Vector3 return types

class Node
{
public:
    Node(std::string name = "Node", Node* parent = nullptr, std::string tag = "");
    virtual ~Node() = default;

    // --- Hierarchy ---
    void AddChild(std::unique_ptr<Node> pChild);
    Node* GetChild(size_t index);
    Node* GetParent() const;
    const std::vector<std::unique_ptr<Node>>& GetChildren() const;
    std::vector<std::unique_ptr<Node>>& GetChildren_NonConst();
    const std::string& GetName() const;
    Node* FindFirstChildByTag(const std::string& searchTag);
    std::vector<Node*> FindAllChildrenByTag(const std::string& searchTag);
    Node* GetRoot() const;

    // --- Components ---
    // ... (GetComponent, AddComponent remain the same) ...
    template<typename T> T* GetComponent() const; // Keep declaration
    Component* AddComponent(std::unique_ptr<Component> pComponent); // Keep declaration
    const std::vector<std::unique_ptr<Component>>& GetComponents() const; // Keep declaration
    void ClearComponents();

    // --- Transform ---
    // Local
    void SetLocalTransform(DirectX::FXMMATRIX transform);      // Recalculates stored pos/quat/scale from matrix
    void SetLocalPosition(const DirectX::XMFLOAT3& pos);       // Updates stored position & marks matrix dirty
    void PhysicsSetLocalPosition(const DirectX::XMFLOAT3& pos);
    void SetLocalRotation(const DirectX::XMFLOAT3& rotRad);    // Updates stored Quaternion from Euler (Pitch, Yaw, Roll in Radians) & marks matrix dirty
    void SetLocalRotation(const DirectX::XMFLOAT4& quat);      // Updates stored Quaternion directly & marks matrix dirty
    void SetWorldRotation(const DirectX::XMFLOAT4& worldQuat);
    void PhysicsSetWorldRotation(const DirectX::XMFLOAT4& worldQuat);
    void PhysicsSetLocalRotation(const DirectX::XMFLOAT4& quat);
    void SetLocalScale(const DirectX::XMFLOAT3& scale);        // Updates stored scale & marks matrix dirty
    void TranslateLocal(const DirectX::XMFLOAT3& translation); // Translates along local axes

    DirectX::XMMATRIX GetLocalTransform() const;               // Constructs matrix from stored components if dirty
    DirectX::XMFLOAT3 GetLocalPosition() const;
    DirectX::XMFLOAT3 GetLocalRotationEuler() const;           // Converts stored Quaternion to Euler (Pitch, Yaw, Roll in Radians)
    DirectX::XMFLOAT4 GetLocalRotationQuaternion() const;      // Returns stored Quaternion
    DirectX::XMFLOAT4 GetWorldRotationQuaternion() const;
    DirectX::XMFLOAT3 GetLocalScale() const;
    DirectX::SimpleMath::Vector3 Forward() const;
    DirectX::SimpleMath::Vector3 Back() const;
    DirectX::SimpleMath::Vector3 Right() const;
    DirectX::SimpleMath::Vector3 Left() const;
    DirectX::SimpleMath::Vector3 Up() const;
    DirectX::SimpleMath::Vector3 Down() const;

    // World
    DirectX::XMMATRIX GetWorldTransform() const;
    DirectX::XMFLOAT3 GetWorldPosition() const;
    void SetWorldPosition(const DirectX::XMFLOAT3& pos);
    void PhysicsSetWorldPosition(const DirectX::XMFLOAT3& pos);
    int myCounter = 0;

    // --- Update & Draw ---
    void Update(float dt);
    void Submit(FrameCommander& frame, Graphics& gfx) const;
    void ShowNodeTree(Node*& pSelectedNode) noexcept;
    std::string tag;
    Node* parent = nullptr;

    bool isMarkedForDeletion = false;
    void Destroy(); // Marks this node and its children for destruction
    bool IsMarkedForDestruction() const;
    void RemoveChild(Node* childToRemove); // Helper to remove a specific child
private:
    void UpdateWorldTransform(bool transformationOutsidePhysicsTriggered);
    void UpdateLocalTransformFromComponents(bool transformationOutsidePhysicsTriggered);     // Helper to build matrix from stored pos/quat/scale
    void UpdateStoredComponentsFromMatrix();       // Helper used by SetLocalTransform (updates pos, quat, scale)

    std::string name;
    bool transformationOutsidePhysicsTriggered = false;

    // --- Store transform components directly ---
    DirectX::XMFLOAT3 localPosition = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT4 localRotationQuaternion = { 0.0f, 0.0f, 0.0f, 1.0f }; // Store as Quaternion (x, y, z, w - identity)
    DirectX::XMFLOAT3 localScale = { 1.0f, 1.0f, 1.0f };

    // --- Matrix caches ---
    DirectX::XMFLOAT4X4 localTransform;
    DirectX::XMFLOAT4X4 worldTransform;

    std::vector<std::unique_ptr<Node>> children;
    std::vector<std::unique_ptr<Component>> components;

    bool localTransformDirty = true; // Flag to rebuild local matrix
    bool worldTransformDirty = true; // Flag to recalculate world matrix
    bool markedForDestruction = false;
};

template<typename T> T* Node::GetComponent() const
{
    static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
    for (const auto& comp : components)
    {
        if (auto* p = dynamic_cast<T*>(comp.get()))
        {
            return p;
        }
    }
    return nullptr;
}