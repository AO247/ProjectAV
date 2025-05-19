#include "Prefab.h"  
#include "PrefabManager.h"  
#include "ModelComponent.h"  
#include "Throwable.h"  
#include "Graphics.h"  
#include "Window.h"  
#include "App.h"  

Prefab::Prefab(const std::string& modelPath, PhysicsEngine* engine, Window* window)
    : modelPath(modelPath), physicsEngine(engine), wnd(window)
{  
   // Ensure the constructor initializes the member variables correctly  
   if (!engine || !window) {  
       throw std::invalid_argument("PhysicsEngine and Window pointers cannot be null.");  
   }  
}  

Prefab::Prefab()  
   : physicsEngine(nullptr), wnd(nullptr) // Ensure default constructor initializes members  
{  
}  

Prefab::~Prefab()  
{  
}  

const std::string& Prefab::GetModelPath() const  
{  
   return modelPath;  
}  

void Prefab::SetModelPath(const std::string& path)  
{  
   modelPath = path;  
}  

Node* Prefab::InstantiateStone(const std::string& prefabName, Node* parentNode, float locX, float locY, float locZ, float scale) const {  
   
    const Prefab* prefab = PrefabManager::GetInstance().GetPrefab(prefabName);  
   if (!prefab) {  
       OutputDebugStringA(("Prefab not found: " + prefabName).c_str());  
       return nullptr;  
   }  

   auto pNewNodeOwner = std::make_unique<Node>(prefabName, nullptr, "Stone");  

   if (!prefab->GetModelPath().empty()) {  
       pNewNodeOwner->AddComponent(  
           std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), prefab->GetModelPath())  
       );  
   }  

   pNewNodeOwner->AddComponent(  
       std::make_unique<Rigidbody>(pNewNodeOwner.get(), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f))  
   );  
   Rigidbody* pRigidbody = pNewNodeOwner->GetComponent<Rigidbody>();  
   pNewNodeOwner->AddComponent(  
       std::make_unique<Throwable>(pNewNodeOwner.get())  
   );  
   pNewNodeOwner->AddComponent(  
       std::make_unique<OBB>(pNewNodeOwner.get(), pRigidbody, Vector3(0.0f, 0.6f, 0.0f), Vector3(1.2f, 1.1f, 1.7f))  
   );  
   OBB* pOBB = pNewNodeOwner->GetComponent<OBB>();  
   pOBB->SetLayer(Layers::THROWABLE);  
   pRigidbody->SetCollider(pOBB);  
   physicsEngine->AddRigidbody(pRigidbody);  
   pNewNodeOwner->AddComponent(  
       std::make_unique<BoundingSphere>(pNewNodeOwner.get(), Vector3(0.0f, 0.6f, 0.0f), 1.5f, nullptr)  
   );  
   BoundingSphere* pBoundingSphere = pNewNodeOwner->GetComponent<BoundingSphere>();  
   pBoundingSphere->SetLayer(Layers::THROWABLE);  
   pBoundingSphere->SetIsTrigger(true);  
   pBoundingSphere->SetTriggerEnabled(true);  
   pNewNodeOwner->GetComponent<Throwable>()->damageArea = pBoundingSphere;  
   physicsEngine->AddCollider(pBoundingSphere);  

   pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
   pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

   Node* pNewNode = pNewNodeOwner.get();  
   parentNode->AddChild(std::move(pNewNodeOwner));  

   return pNewNode;  
}
