#pragma once
#include <vector>
#include <memory>
#include <string>
#include <DirectXMath.h>
#include <typeinfo> 
#include "Component.h"
#include <SimpleMath.h> 

class Graphics;
class TechniqueProbe;
class ModelProbe;
class Component;

using namespace DirectX::SimpleMath; 

class Node
{
public:
    Node(std::string name = "Node", Node* parent = nullptr, std::string tag = "");
    virtual ~Node() = default;

    void AddChild(std::unique_ptr<Node> pChild);
    Node* GetChild(size_t index);
    Node* GetParent() const;
    void SetParent(Node* newParent);
    const std::vector<std::unique_ptr<Node>>& GetChildren() const;
    std::vector<std::unique_ptr<Node>>& GetChildren_NonConst();
    const std::string& GetName() const;
    Node* FindFirstChildByTag(const std::string& searchTag);
    std::vector<Node*> FindAllChildrenByTag(const std::string& searchTag);
    Node* GetRoot() const;
    float radius = 60.0f;

    template<typename T> T* GetComponent() const; 
    Component* AddComponent(std::unique_ptr<Component> pComponent);
    const std::vector<std::unique_ptr<Component>>& GetComponents() const; 
    void ClearComponents();


    void SetLocalTransform(DirectX::FXMMATRIX transform);      
    void SetLocalPosition(const DirectX::XMFLOAT3& pos);       
    void PhysicsSetLocalPosition(const DirectX::XMFLOAT3& pos);
    void SetLocalRotation(const DirectX::XMFLOAT3& rotRad);    
    void SetLocalRotation(const DirectX::XMFLOAT4& quat);      
    void SetWorldRotation(const DirectX::XMFLOAT4& worldQuat);
    void PhysicsSetWorldRotation(const DirectX::XMFLOAT4& worldQuat);
    void PhysicsSetLocalRotation(const DirectX::XMFLOAT4& quat);
    void SetLocalScale(const DirectX::XMFLOAT3& scale);        
    void TranslateLocal(const DirectX::XMFLOAT3& translation); 

    DirectX::XMMATRIX GetLocalTransform() const;               
    DirectX::XMFLOAT3 GetLocalPosition() const;
    DirectX::XMFLOAT3 GetLocalRotationEuler() const;           
    DirectX::XMFLOAT4 GetLocalRotationQuaternion() const;      
    DirectX::XMFLOAT4 GetWorldRotationQuaternion() const;
    DirectX::XMFLOAT3 GetLocalScale() const;
    DirectX::SimpleMath::Vector3 Forward() const;
    DirectX::SimpleMath::Vector3 Back() const;
    DirectX::SimpleMath::Vector3 Right() const;
    DirectX::SimpleMath::Vector3 Left() const;
    DirectX::SimpleMath::Vector3 Up() const;
    DirectX::SimpleMath::Vector3 Down() const;

    DirectX::XMMATRIX GetWorldTransform() const;
    DirectX::XMFLOAT3 GetWorldPosition() const;
    void SetWorldPosition(const DirectX::XMFLOAT3& pos);
    void PhysicsSetWorldPosition(const DirectX::XMFLOAT3& pos);
    int myCounter = 0;

    void Update(float dt);
    void Submit(Graphics& gfx) const;
    void ShowNodeTree(Node*& pSelectedNode) noexcept;
    std::string tag;
    Node* parent = nullptr;

    bool isMarkedForDeletion = false;
    bool markedForDestruction = false;

    void Destroy();
    bool IsMarkedForDestruction() const;
    void RemoveChild(Node* childToRemove);
    void MoveToTop();
    std::vector<std::unique_ptr<Node>> children;

private:
    void UpdateWorldTransform(bool transformationOutsidePhysicsTriggered);
    void UpdateLocalTransformFromComponents(bool transformationOutsidePhysicsTriggered);  
    void UpdateStoredComponentsFromMatrix();    

    std::string name;
    bool transformationOutsidePhysicsTriggered = false;

    DirectX::XMFLOAT3 localPosition = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT4 localRotationQuaternion = { 0.0f, 0.0f, 0.0f, 1.0f };
    DirectX::XMFLOAT3 localScale = { 1.0f, 1.0f, 1.0f };

    DirectX::XMFLOAT4X4 localTransform;
    DirectX::XMFLOAT4X4 worldTransform;

    std::vector<std::unique_ptr<Component>> components;

    bool localTransformDirty = true; 
    bool worldTransformDirty = true; 
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