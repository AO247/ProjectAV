//#pragma once
//#include <vector>
//#include <memory>
//#include <string>
//#include <DirectXMath.h>
//#include <typeinfo> // For GetComponent
//#include "Component.h"
//class AIController : public Component
//{
//public:
//    AIController(Node* owner);
//    virtual ~AIController() noexcept;
//    
//    void Update(float dt);
//
//    template<typename T> T* GetComponent() const;
//    Component* AddComponent(std::unique_ptr<Component> pComponent);
//    const std::vector<std::unique_ptr<Component>>& GetComponents() const;
//
//    std::vector<std::unique_ptr<Component>> components;
//};