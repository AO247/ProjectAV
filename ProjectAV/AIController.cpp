#include "AIController.h"

AIController::AIController(Node* owner)
    :Component(owner)
{
}


void AIController::Update(float dt)
{
   
}





template<typename T> T* AIController::GetComponent() const
{
    static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
    for (const auto& comp : components)
    {
        if (auto* p = dynamic_cast<T*>(comp.get()))
        {
            return p;
        }
    }
    return nullptr;
}
Component* AIController::AddComponent(std::unique_ptr<Component> pComponent) {
    assert(pComponent);
    components.push_back(std::move(pComponent));
    return components.back().get();
}
const std::vector<std::unique_ptr<Component>>& AIController::GetComponents() const { return components; }

