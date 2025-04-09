#pragma once
#pragma once
#include <vector>
#include <memory>
#include <string>
#include <DirectXMath.h>
#include <typeinfo> // For GetComponent
#include "Component.h"

class Graphics; // Forward declaration

class Node
{
public:
    Node(std::string name = "Node", Node* parent = nullptr);
    virtual ~Node() = default;

    // --- Hierarchy ---
    void AddChild(std::unique_ptr<Node> pChild);
    Node* GetChild(size_t index);
    Node* GetParent() const;
    const std::vector<std::unique_ptr<Node>>& GetChildren() const;
    const std::string& GetName() const;

    // --- Components ---
    template<typename T>
    T* GetComponent() const
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

    // Takes ownership of the component
    Component* AddComponent(std::unique_ptr<Component> pComponent);

    // --- Transform ---
    // Set local transform relative to parent
    void SetLocalTransform(DirectX::FXMMATRIX transform);
    void SetLocalPosition(const DirectX::XMFLOAT3& pos);
    void SetLocalRotation(const DirectX::XMFLOAT3& rotRad); // Euler angles in radians
    void SetLocalScale(const DirectX::XMFLOAT3& scale);
    void SetWorldPosition(const DirectX::XMFLOAT3& pos); // <--- ADDED

    DirectX::XMMATRIX GetLocalTransform() const;
    DirectX::XMMATRIX GetWorldTransform() const; // Calculates on the fly or caches
    DirectX::XMFLOAT3 GetWorldPosition() const; // <--- ADDED

    DirectX::XMFLOAT3 GetLocalPosition() const;
    DirectX::XMFLOAT3 GetLocalRotationEuler() const; // Returns Pitch, Yaw, Roll in RADIANS
    DirectX::XMFLOAT3 GetLocalScale() const;

    const std::vector<std::unique_ptr<Component>>& GetComponents() const;

    // --- Update & Draw ---
    // Updates world transform based on parent and local, then updates children and components
    void Update(float dt);
    // Draws components of this node and then children (basic scene graph traversal)
    void Draw(Graphics& gfx) const;
    void ShowNodeTree(Node*& pSelectedNode) noexcept;


private:
    void UpdateWorldTransform(); // Helper to update matrix based on parent

    std::string name;
    DirectX::XMFLOAT4X4 localTransform;
    DirectX::XMFLOAT4X4 worldTransform; // Cached world transform

    Node* parent = nullptr; // Non-owning pointer
    std::vector<std::unique_ptr<Node>> children;
    std::vector<std::unique_ptr<Component>> components;

    bool worldTransformDirty = true; // Flag to recalculate world transform
};