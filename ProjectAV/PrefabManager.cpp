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

Node* PrefabManager::InstantiateIsland(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Island", nullptr, "Ground");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\wyspa\\wyspa_test.fbx")
    );
    pNewNodeOwner->AddComponent(
        std::make_unique<OBB>(pNewNodeOwner.get(), nullptr, Vector3(0.0f, -0.3f, 0.0f), Vector3(40.0f, 1.0f, 40.0f))
    );
    OBB* pOBB = pNewNodeOwner->GetComponent<OBB>();
    pOBB->SetLayer(Layers::GROUND);
    physicsEngine->AddCollider(pOBB);

	pNewNodeOwner->AddComponent(
		std::make_unique<Island>(pNewNodeOwner.get())
	);
	Island* pIsland = pNewNodeOwner->GetComponent<Island>();
    auto leftPoint = std::make_unique<Node>("Left Point", pNewNodeOwner.get());
	auto rightPoint = std::make_unique<Node>("Right Point", pNewNodeOwner.get());
	auto upPoint = std::make_unique<Node>("Up Point", pNewNodeOwner.get());
	auto downPoint = std::make_unique<Node>("Down Point", pNewNodeOwner.get());

	leftPoint->SetLocalPosition(DirectX::XMFLOAT3(24.0f, 0.0f, 5.0f));
	rightPoint->SetLocalPosition(DirectX::XMFLOAT3(-24.0f, 0.0f, -10.0f));
	upPoint->SetLocalPosition(DirectX::XMFLOAT3(10.0f, 0.0f, 24.0f));
	downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -24.0f));
    pIsland->leftPoint = leftPoint.get();
	pIsland->rightPoint = rightPoint.get();
	pIsland->upPoint = upPoint.get();
	pIsland->downPoint = downPoint.get();
    pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
    pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
    
    Node* pNewNode = pNewNodeOwner.get();

    //Node* stone = InstantiateStone(pNewNode, pNewNode->GetWorldPosition().x + 5.0f, 30.0f, 0.0f, 1.5f);

	pNewNode->AddChild(std::move(leftPoint));
	pNewNode->AddChild(std::move(rightPoint));
	pNewNode->AddChild(std::move(upPoint));
	pNewNode->AddChild(std::move(downPoint));
    parentNode->AddChild(std::move(pNewNodeOwner));
    return pNewNode;
}