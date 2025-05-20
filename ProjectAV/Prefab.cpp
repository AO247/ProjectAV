//#include "Prefab.h"  
//#include "PrefabManager.h"  
//#include "ModelComponent.h"  
//#include "Throwable.h"  
//#include "Graphics.h"  
//#include "Window.h"  
//#include "App.h"  
//
//Prefab::Prefab(PhysicsEngine* engine, Window* window)
//    : physicsEngine(engine), wnd(window)
//{  
//   // Ensure the constructor initializes the member variables correctly  
//   if (!engine || !window) {  
//       throw std::invalid_argument("PhysicsEngine and Window pointers cannot be null.");  
//   }  
//}  
//
//Prefab::Prefab()  
//   : physicsEngine(nullptr), wnd(nullptr) // Ensure default constructor initializes members  
//{  
//}  
//
//Prefab::~Prefab()  
//{  
//}  
//
//const std::string& Prefab::GetModelPath() const  
//{  
//   return modelPath;  
//}  
//
//void Prefab::SetModelPath(const std::string& path)  
//{  
//   modelPath = path;  
//}  
//
//
//
//Node* Prefab::InstantiateColumn(Node* parentNode, float locX, float locY, float locZ, float scale) const {
//    const std::string prefabName = "Column";
//    const Prefab* prefab = PrefabManager::GetInstance().GetPrefab(prefabName);
//
//    if (!prefab) {
//        OutputDebugStringA(("Prefab not found: " + prefabName).c_str());
//        return nullptr;
//    }
//
//    auto pNewNodeOwner = std::make_unique<Node>(prefabName, nullptr, "Column");
//
//    if (!prefab->GetModelPath().empty()) {
//        pNewNodeOwner->AddComponent(
//            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), prefab->GetModelPath())
//        );
//    }
//
//    return prefab->InstantiateStone();
//}