#include "PrefabManager.h"
#include "Components.h"
#include "Node.h"


PrefabManager::PrefabManager(PhysicsEngine* pe, Window* wnd)
    : physicsEngine(pe), wnd(wnd) {
    
}

PrefabManager::~PrefabManager()
{
}

Node* PrefabManager::InstantiateStone(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "Stone");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\kamien\\kamien_6.obj")
    );

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