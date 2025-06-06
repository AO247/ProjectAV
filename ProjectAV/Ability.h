#pragma once
#include "Component.h" // Jeœli umiejêtnoœci s¹ komponentami
#include "Window.h"
class Node; // Forward declaration

class Ability : public Component // Lub dziedzicz po prostu z niczego, jeœli nie wszystkie umiejêtnoœci to komponenty
{
public:
    Ability(Node* owner, Window& window, Node* camera) // Przyk³adowy konstruktor
        : Component(owner), wnd(window), camera(camera) {
    } // Jeœli dziedziedzisz z Component

    virtual ~Ability() = default;

    // Metody, które ka¿da umiejêtnoœæ musi zaimplementowaæ
    virtual void Pressed() = 0;   // Czysto wirtualna, jeœli ka¿da umiejêtnoœæ MUSI to zdefiniowaæ
    virtual void Released() = 0;
    virtual void Update(float dt) override // Jeœli dziedziczysz z Component i chcesz zachowaæ spójnoœæ
    {
        Component::Update(dt); // Wywo³aj bazow¹ implementacjê jeœli istnieje
    }
    std::vector<Node*> objects;
protected:
    Window& wnd;
    Node* camera;
};