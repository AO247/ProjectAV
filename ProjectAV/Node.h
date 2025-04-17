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
    // ... (AddChild, GetChild, GetParent, GetChildren, GetName remain the same) ...
    void AddChild(std::unique_ptr<Node> pChild);
    Node* GetChild(size_t index);
    Node* GetParent() const;
    const std::vector<std::unique_ptr<Node>>& GetChildren() const;
    const std::string& GetName() const;

    // --- Components ---
    // ... (GetComponent, AddComponent remain the same) ...
    template<typename T> T* GetComponent() const; // Keep declaration
    Component* AddComponent(std::unique_ptr<Component> pComponent); // Keep declaration
    const std::vector<std::unique_ptr<Component>>& GetComponents() const; // Keep declaration

    // --- Transform ---
    // Local
    void SetLocalTransform(DirectX::FXMMATRIX transform); // Recalculates stored Euler from matrix
    void SetLocalPosition(const DirectX::XMFLOAT3& pos);  // Updates matrix using stored Euler
    void SetLocalRotation(const DirectX::XMFLOAT3& rotRad); // Updates stored Euler & matrix
    void SetLocalScale(const DirectX::XMFLOAT3& scale);    // Updates matrix using stored Euler

    DirectX::XMMATRIX GetLocalTransform() const; // Constructs matrix from stored components
    DirectX::XMFLOAT3 GetLocalPosition() const;
    DirectX::XMFLOAT3 GetLocalRotationEuler() const; // Returns stored Euler angles
    DirectX::XMFLOAT3 GetLocalScale() const;

    // World
    DirectX::XMMATRIX GetWorldTransform() const;
    DirectX::XMFLOAT3 GetWorldPosition() const;
    void SetWorldPosition(const DirectX::XMFLOAT3& pos); // Adjusts local pos based on parent

    // --- Update & Draw ---
    void Update(float dt);
    void Draw(Graphics& gfx) const;
    void ShowNodeTree(Node*& pSelectedNode) noexcept;

private:
    void UpdateWorldTransform();
    void UpdateLocalTransformFromComponents(); // Helper to build matrix from stored pos/rot/scale
    void UpdateStoredEulerFromMatrix();        // Helper used by SetLocalTransform

    std::string name;

    // --- Store transform components directly ---
    DirectX::XMFLOAT3 localPosition = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 localRotationEulerRad = { 0.0f, 0.0f, 0.0f }; // Store Pitch, Yaw, Roll in Radians
    DirectX::XMFLOAT3 localScale = { 1.0f, 1.0f, 1.0f };

    // --- Matrix caches ---
    DirectX::XMFLOAT4X4 localTransform;  // Cache built from pos/rot/scale
    DirectX::XMFLOAT4X4 worldTransform; // Cache built from local and parent

    Node* parent = nullptr;
    std::vector<std::unique_ptr<Node>> children;
    std::vector<std::unique_ptr<Component>> components;

    bool localTransformDirty = true; // Flag to rebuild local matrix
    bool worldTransformDirty = true; // Flag to recalculate world matrix
};

// Template implementations if needed in header
