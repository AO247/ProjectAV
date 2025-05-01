//#include "AIController.h"
//
//AIController::AIController(Node* owner)
//    :Component(owner)
//{
//	pPlayerNode = owner->GetRoot()->FindFirstChildByTag("Player");        
//}
//
//
//void AIController::Update(float dt)
//{
//   
//}
//
//Component* AIController::AddComponent(std::unique_ptr<Component> pComponent) {
//    assert(pComponent);
//    components.push_back(std::move(pComponent));
//    return components.back().get();
//}
//const std::vector<std::unique_ptr<Component>>& AIController::GetComponents() const { return components; }
//
//
