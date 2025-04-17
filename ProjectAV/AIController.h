#pragma once
#include <vector>
#include <memory>
#include <string>
#include <DirectXMath.h>
#include <typeinfo> // For GetComponent
#include "Component.h"
class AIController : public Component
{
public:
    AIController(Node* owner);
    virtual ~AIController() noexcept; // Need destructor to manage pImpl
    
    void Update(float dt);

    template<typename T> T* GetComponent() const; // Keep declaration
    Component* AddComponent(std::unique_ptr<Component> pComponent); // Keep declaration
    const std::vector<std::unique_ptr<Component>>& GetComponents() const; // Keep declaration

    std::string tag;
    std::vector<std::unique_ptr<Component>> components;

};