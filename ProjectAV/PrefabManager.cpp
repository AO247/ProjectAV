#include "PrefabManager.h"
#include "Components.h"
#include "PhysicsCommon.h"

PrefabManager::~PrefabManager()
{
}

Node* PrefabManager::InstantiateStone(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "Stone");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\kamien\\kamien_6.obj")
    );

    /*pNewNodeOwner->AddComponent(
        std::make_unique<Rigidbody>(pNewNodeOwner.get(), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f))
    );
    Rigidbody* pRigidbody = pNewNodeOwner->GetComponent<Rigidbody>();*/
    pNewNodeOwner->AddComponent(
        std::make_unique<Throwable>(pNewNodeOwner.get())
    );
    /*pNewNodeOwner->AddComponent(
        std::make_unique<OBB>(pNewNodeOwner.get(), pRigidbody, Vector3(0.0f, 0.6f, 0.0f), Vector3(1.2f, 1.1f, 1.7f))
    );*/
    /*OBB* pOBB = pNewNodeOwner->GetComponent<OBB>();
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
    physicsEngine->AddCollider(pBoundingSphere);*/

    pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
    pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

    Node* pNewNode = pNewNodeOwner.get();
    parentNode->AddChild(std::move(pNewNodeOwner));
    return pNewNode;
}

Node* PrefabManager::InstantiateIslandBig1(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Island", nullptr, "Ground");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\wyspa\\wyspa_test.fbx")
    );
    ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
    TriangleList islandTriangles = PhysicsCommon::MakeTriangleList(islandModel->GetAllTriangles());
    MeshShapeSettings islandMeshSettings(islandTriangles);
    Shape::ShapeResult islandMeshCreationResult = islandMeshSettings.Create();
    ShapeRefC islandMeshShape = islandMeshCreationResult.Get();
    ScaledShapeSettings islandScaling(islandMeshShape, Vec3Arg(scale, scale, scale));
    islandMeshShape = islandScaling.Create().Get();
    BodyCreationSettings bodySettings(islandMeshShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
    bodySettings.mFriction = 1.0f;
    pNewNodeOwner->AddComponent(
        std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
    );

	pNewNodeOwner->AddComponent(
		std::make_unique<Island>(pNewNodeOwner.get())
	);
	Island* pIsland = pNewNodeOwner->GetComponent<Island>();
    auto leftPoint = std::make_unique<Node>("Left Point", pNewNodeOwner.get());
	auto rightPoint = std::make_unique<Node>("Right Point", pNewNodeOwner.get());
	auto upPoint = std::make_unique<Node>("Up Point", pNewNodeOwner.get());
	auto downPoint = std::make_unique<Node>("Down Point", pNewNodeOwner.get());

	leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-24.0f, 0.0f, 5.0f));
	rightPoint->SetLocalPosition(DirectX::XMFLOAT3(24.0f, 0.0f, -10.0f));
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

Node* PrefabManager::InstantiateIslandMedium1(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Island", nullptr, "Ground");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\objects\\wyspa_wycieta.obj")
    );
    ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
    TriangleList islandTriangles = PhysicsCommon::MakeTriangleList(islandModel->GetAllTriangles());
    MeshShapeSettings islandMeshSettings(islandTriangles);
    Shape::ShapeResult islandMeshCreationResult = islandMeshSettings.Create();
    ShapeRefC islandMeshShape = islandMeshCreationResult.Get();
    ScaledShapeSettings islandScaling(islandMeshShape, Vec3Arg(scale, scale, scale));
    islandMeshShape = islandScaling.Create().Get();
    BodyCreationSettings bodySettings(islandMeshShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
    bodySettings.mFriction = 1.0f;
    pNewNodeOwner->AddComponent(
        std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
    );

    pNewNodeOwner->AddComponent(
        std::make_unique<Island>(pNewNodeOwner.get())
    );
    Island* pIsland = pNewNodeOwner->GetComponent<Island>();
    auto leftPoint = std::make_unique<Node>("Left Point", pNewNodeOwner.get());
    auto rightPoint = std::make_unique<Node>("Right Point", pNewNodeOwner.get());
    auto upPoint = std::make_unique<Node>("Up Point", pNewNodeOwner.get());
    auto downPoint = std::make_unique<Node>("Down Point", pNewNodeOwner.get());

    upPoint->SetLocalPosition(DirectX::XMFLOAT3(3.0f, 0.0f, 11.0f));
    downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -11.0f));
    leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-9.0f, 0.0f, 5.0f));
    rightPoint->SetLocalPosition(DirectX::XMFLOAT3(11.0f, 0.0f, -4.0f));

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


Node* PrefabManager::InstantiateIslandMedium2(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Island", nullptr, "Ground");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\objects\\wyspa_srednia_1.obj")
    );
    ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
    TriangleList islandTriangles = PhysicsCommon::MakeTriangleList(islandModel->GetAllTriangles());
    MeshShapeSettings islandMeshSettings(islandTriangles);
    Shape::ShapeResult islandMeshCreationResult = islandMeshSettings.Create();
    ShapeRefC islandMeshShape = islandMeshCreationResult.Get();
    ScaledShapeSettings islandScaling(islandMeshShape, Vec3Arg(scale, scale, scale));
    islandMeshShape = islandScaling.Create().Get();
    BodyCreationSettings bodySettings(islandMeshShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
    bodySettings.mFriction = 1.0f;
    pNewNodeOwner->AddComponent(
        std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
    );

    pNewNodeOwner->AddComponent(
        std::make_unique<Island>(pNewNodeOwner.get())
    );
    Island* pIsland = pNewNodeOwner->GetComponent<Island>();
    auto leftPoint = std::make_unique<Node>("Left Point", pNewNodeOwner.get());
    auto rightPoint = std::make_unique<Node>("Right Point", pNewNodeOwner.get());
    auto upPoint = std::make_unique<Node>("Up Point", pNewNodeOwner.get());
    auto downPoint = std::make_unique<Node>("Down Point", pNewNodeOwner.get());

    upPoint->SetLocalPosition(DirectX::XMFLOAT3(3.0f, 0.0f, 11.0f));
    downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -11.0f));
    leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-9.0f, 0.0f, 5.0f));
    rightPoint->SetLocalPosition(DirectX::XMFLOAT3(11.0f, 0.0f, -4.0f));

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

Node* PrefabManager::InstantiateIslandSmall1(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Island", nullptr, "Ground");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\objects\\wyspa_mala_1.obj")
    );
    ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
    TriangleList islandTriangles = PhysicsCommon::MakeTriangleList(islandModel->GetAllTriangles());
    MeshShapeSettings islandMeshSettings(islandTriangles);
    Shape::ShapeResult islandMeshCreationResult = islandMeshSettings.Create();
    ShapeRefC islandMeshShape = islandMeshCreationResult.Get();
    ScaledShapeSettings islandScaling(islandMeshShape, Vec3Arg(scale, scale, scale));
    islandMeshShape = islandScaling.Create().Get();
    BodyCreationSettings bodySettings(islandMeshShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
    bodySettings.mFriction = 1.0f;
    pNewNodeOwner->AddComponent(
        std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
    );

    pNewNodeOwner->AddComponent(
        std::make_unique<Island>(pNewNodeOwner.get())
    );
    Island* pIsland = pNewNodeOwner->GetComponent<Island>();
    auto leftPoint = std::make_unique<Node>("Left Point", pNewNodeOwner.get());
    auto rightPoint = std::make_unique<Node>("Right Point", pNewNodeOwner.get());
    auto upPoint = std::make_unique<Node>("Up Point", pNewNodeOwner.get());
    auto downPoint = std::make_unique<Node>("Down Point", pNewNodeOwner.get());

    upPoint->SetLocalPosition(DirectX::XMFLOAT3(3.0f, 0.0f, 11.0f));
    downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -11.0f));
    leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-9.0f, 0.0f, 5.0f));
    rightPoint->SetLocalPosition(DirectX::XMFLOAT3(11.0f, 0.0f, -4.0f));

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