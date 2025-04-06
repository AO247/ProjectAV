#pragma once
#include <memory>

// Forward declarations
class Node;
class Graphics;
namespace DirectX { struct XMMATRIX; } // Use struct forward declaration

class Component
{
public:
    Component(Node* owner) : pOwner(owner) {}
    virtual ~Component() = default;

    // Common methods components might override (optional for now)
    virtual void Update(float dt) {}
    // Draw might need specific parameters depending on the component type
    // For now, ModelComponent will have its own Draw with needed params.

    Node* GetOwner() const { return pOwner; }

protected:
    Node* pOwner; // Non-owning pointer to the Node this component is attached to
};