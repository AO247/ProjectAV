//#pragma once
//#include <vector>
//#include <memory>
//#include <string>
//#include <DirectXMath.h>
//#include <typeinfo> // For GetComponent
//#include "Component.h"
//#include "Node.h"
//class AIController : public Component
//{
//public:
//    AIController(Node* owner);
//	~AIController() = default;
//    void Update(float dt);
//
//    template<typename T> T* GetComponent() const;
//    Component* AddComponent(std::unique_ptr<Component> pComponent);
//    const std::vector<std::unique_ptr<Component>>& GetComponents() const;
//
//    std::vector<std::unique_ptr<Component>> components;
//	Node* pPlayerNode = nullptr; // Node representing the player capsule/origin
//
//};
//template<typename T> T* AIController::GetComponent() const
//{
//    static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
//    for (const auto& comp : components)
//    {
//        if (auto* p = dynamic_cast<T*>(comp.get()))
//        {
//            return p;
//        }
//    }
//    return nullptr;
//}