#pragma once
#include "Component.h" // Je�li umiej�tno�ci s� komponentami
#include "Window.h"
class Node; // Forward declaration

class Ability : public Component // Lub dziedzicz po prostu z niczego, je�li nie wszystkie umiej�tno�ci to komponenty
{
public:
    Ability(Node* owner, Window& window, Node* camera) // Przyk�adowy konstruktor
        : Component(owner), wnd(window), camera(camera) {
    } // Je�li dziedziedzisz z Component

    virtual ~Ability() = default;

    // Metody, kt�re ka�da umiej�tno�� musi zaimplementowa�
    virtual void Pressed() = 0;   // Czysto wirtualna, je�li ka�da umiej�tno�� MUSI to zdefiniowa�
    virtual void Released() = 0;
    virtual void Update(float dt) override // Je�li dziedziczysz z Component i chcesz zachowa� sp�jno��
    {
        Component::Update(dt); // Wywo�aj bazow� implementacj� je�li istnieje
    }
    std::vector<Node*> objects;
protected:
    Window& wnd;
    Node* camera;
};