#pragma once
#include <memory>
#include "imgui/imgui.h"
#include <string>
#include <DirectXMath.h>
#include "Node.h"

class Node;
class Graphics;
namespace DirectX { struct XMMATRIX; }

class Component
{
public:
    Component(Node* owner, std::string tag = "")
        : pOwner(owner), tag(std::move(tag)){}
    virtual ~Component() = default;

    virtual void Update(float dt) {}

    virtual void OnTriggerEnter(Node* object);
    virtual void OnTriggerStay(const std::vector<Node*> others);
    virtual void OnTriggerExit(Node* object);
    virtual void OnCollisionEnter(Node* object);
    virtual void OnCollisionStay(Node* object);
    virtual void OnCollisionExit(Node* object);

    virtual void Follow(float dt, DirectX::XMFLOAT3 target, float sp = 1.0f) {}
	virtual void Attack(float dt) {}
    bool canAttack = true;
    virtual bool GroundCheck() { return false; }
    Node* GetOwner() const { return pOwner; }
    virtual void DrawImGuiControls() {};
    std::string tag;
    bool isRigidbody = false;
    float attackRange;
    float flyingHeight;
    bool endAttack;

protected:
    Node* pOwner;
};