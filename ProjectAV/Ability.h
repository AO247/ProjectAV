#pragma once
#include "Component.h"
#include "Window.h"

class Node;

class Ability : public Component
{
public:
    Ability(Node* owner, Window& window, Node* camera)
        : Component(owner), wnd(window), camera(camera)
    {
        // W konstruktorze bazowym mo¿emy pobraæ, jeœli potrzeba, np. wskazanie na "player"
        player = owner->GetRoot()->FindFirstChildByTag("PLAYER");
    }    
    virtual ~Ability() = default;

    virtual void Update(float dt) override;
    virtual void DrawImGuiControls() override;

    // Wirtualne metody do nadpisania w pochodnych klasach
    virtual void Pressed() {}
    virtual void Released() {}
private:
    Window& wnd;
    Node* camera = nullptr;
    Node* player = nullptr;
};