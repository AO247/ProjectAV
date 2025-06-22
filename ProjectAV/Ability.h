#pragma once
#include "Component.h"
#include "Window.h"
class Node;

class Ability : public Component 
{
public:
    Ability(Node* owner, Window& window, Node* camera)
        : Component(owner), wnd(window), camera(camera) {
    }

    virtual ~Ability() = default;

    virtual void Pressed() = 0;
    virtual void Released() = 0;
    virtual void Update(float dt) override
    {
        Component::Update(dt);
    }
    std::vector<Node*> objects;
    float force;
    int killsCount = 3;
    float holdSoundTimer = 0.0f;
    float holdSoundInterval = 2.0f;
protected:
    Window& wnd;
    Node* camera;
};