#pragma once
#include <memory>
#include "imgui/imgui.h"
#include <string>
#include <DirectXMath.h>
#include "Node.h"
// Forward declarations
class Node;
class Graphics;
namespace DirectX { struct XMMATRIX; } // Use struct forward declaration

class Component
{
public:
    Component(Node* owner, std::string tag = "") // Add tag with default
        : pOwner(owner), tag(std::move(tag)){}
    virtual ~Component() = default;

    // Common methods components might override (optional for now)
    virtual void Update(float dt) {}
    // Draw might need specific parameters depending on the component type
    // For now, ModelComponent will have its own Draw with needed params.

    virtual void OnTriggerEnter(Node* object);
    virtual void OnTriggerExit(Node* object);

    virtual void Follow(DirectX::XMFLOAT3 target, float sp = 1.0f) {}
	virtual void Attack(float dt) {} // For components needing Graphics
    bool endAttack;
    Node* GetOwner() const { return pOwner; }
    virtual void DrawImGuiControls() {};
    std::string tag;

protected:
    Node* pOwner; // Non-owning pointer to the Node this component is attached to
};